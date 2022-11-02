import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import pandas as pd
from sklearn.model_selection import train_test_split
import math
import sys
from torch.utils.data.sampler import SubsetRandomSampler
from torch.utils.data import Dataset, DataLoader
from sklearn.preprocessing import MinMaxScaler, StandardScaler, MaxAbsScaler
from torchsummary import summary
import numpy as np
import copy
import random
from collections import OrderedDict
from sklearn.metrics import mean_absolute_error, mean_squared_error, mean_absolute_percentage_error, r2_score
from deepres import DeepRes, Block
from torchlars import LARS
from pytorch_optimizer import LARS
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('--bracket', default="A", choices=["A", "B", "C", "neither"], \
        help='choose from classes A, B, C or neither')

args = parser.parse_args()

device = torch.device("cuda:1" if torch.cuda.is_available() else "cpu")

class PrepareData(Dataset):
    def __init__(self, X, y, train=True, scaler_obj=None):
        #self.sc1 = MinMaxScaler(feature_range=(0,5))
        
        # figure out log scaling for some of the large valued columns
        self.sc1 = StandardScaler()
        #self.sc2 = MinMaxScaler(feature_range=(1,25))
        print("Entering torch.is_tensor(X)\n")
        if not torch.is_tensor(X):
            if train:
                X = self.sc1.fit_transform(X)
                self.X = torch.from_numpy(X)
            else:
                if scaler_obj is not None:
                    X = scaler_obj.transform(X)
                    self.X = torch.from_numpy(X)
                else:
                    print('include scaler object from training')
                    #X = MinMaxScaler(feature_range=(x_min, x_max)).fit_transform(X)
                    #self.X = torch.from_numpy(X)
        print("Entering torch.is_tensor(y)\n")
        if not torch.is_tensor(y):
            y = y.to_numpy()
            # keep this line
            y = np.true_divide(y, 1e6)
            y = np.reshape(y, (-1,1))
            if train:
                #y = self.sc2.fit_transform(y)
                print(type(y.dtype), y.dtype)
                self.y = torch.from_numpy(y)
            else:
                #y = MinMaxScaler(feature_range=(y_min, y_max)).fit_transform(y)
                self.y = torch.from_numpy(y)

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return self.X[idx], self.y[idx]
    
    def return_scaler_obj(self):
        return self.sc1


class KernelRunModel(torch.nn.Module):
    def __init__(self, ip_features, num_hidden, op_features=1):
        super(KernelRunModel, self).__init__()

        self.ip = torch.nn.Linear(ip_features, num_hidden)
        self.hidden2 = torch.nn.Linear(num_hidden, num_hidden*2)
        self.hidden3 = torch.nn.Linear(num_hidden*2, num_hidden)
        #self.hidden4 = torch.nn.Linear(80,num_hidden)
        #self.hidden5 = torch.nn.Linear(40, num_hidden)
        #self.bn1 = nn.BatchNorm1d(num_hidden)
        #self.bn2 = nn.BatchNorm1d(num_hidden*2)
        #self.bn3 = nn.BatchNorm1d(ip_features)
        self.hidden6 = torch.nn.Linear(num_hidden, num_hidden)
        self.hidden7 = torch.nn.Linear(num_hidden, ip_features)
        self.op_run = torch.nn.Linear(ip_features, op_features)
        self.dropout = nn.Dropout(p=0.5)
        self.d2 = nn.Dropout(p=0.1)
    
    def forward(self, x):
        op = F.relu(self.ip(x))
        x = F.relu(self.hidden2(op))
        op2 = F.relu(self.hidden3(x))
        
        x = self.dropout(op2)
        x = F.relu(self.hidden6(op2))
        
        x = F.relu(self.hidden7(x))
        
        x = F.relu(self.op_run(x))
        return x

#dr_columns = ['kernel','Compiler','Cluster','gpu_name'] # using all hardware features except string variables

#dr_columns_old_dataset = ['kernel','Compiler','gpu_name','thread_per_core', 'core_per_socket', 'num_sockets', 'cpu_clock', 'l1i', 'l1d', 'l2', 'l3', 'connector_bandwidth', 'memory_clock', 'memory_bandwidth', 'memory_total', 'sm_clock', 'num_cores', 'threads_per_wrap', 'max_wraps_per_sm', 'max_threads_per_sm', 'max_thread_blocks_per_sm', 'max_32-bit_registers_per_sm', 'max_registers_per_block', 'max_registers_per_thread', 'max_thread_block_size', 'shared_memory_size_per_sm', 'mem_to', 'mem_from','add_sub_int', 'add_sub_float', 'mul_int', 'mul_float', 'div_int', 'div_float', 'bit_rel_logic_int', 'bit_rel_logic_float', 'rem_int', 'assign_int', 'assign_float'] 

# for new dataset created by Alok; run log values on some columns AG -> AY; runtime // 1e6
dr_columns = ['kernel']


#dataset_root="/home/almishra/compoff/algorithms/matrix_vector/variants/"
dataset_root="/home/almishra/git/omp_offload_advisor/model/"
df = pd.read_csv(dataset_root+"dataset_corona.csv")
#df = pd.read_csv(dataset_root+"intel.csv")
#df2 = pd.read_csv(dataset_root+"k80.csv")
#df3 = pd.read_csv(dataset_root+"rtx.csv")
#df4 = pd.read_csv(dataset_root+"summit_llvm.csv")
#df5 = pd.read_csv(dataset_root+"summit_gcc.csv")

#single = pd.concat([df,df2,df3,df4,df5], axis=0)
single = pd.concat([df], axis=0)
single = single.drop(columns=dr_columns)

#print('the original dataset has any null values?', single.isnull().values.any())

#for i in range(len(df.index)) :
#    if (df.iloc[i].isnull().sum() > 0):
#        print("Nan in row ", i , " : " ,  df.iloc[i].isnull().sum())

# replacing 0's with 1 for applying log to those columns
single["memTo"].replace({0:1}, inplace=True)
single["memFrom"].replace({0:1}, inplace=True)
single["memAlloc"].replace({0:1}, inplace=True)
single["memDelete"].replace({0:1}, inplace=True)

#applying log function to 13 columns 

updated_single = single.apply(lambda x: np.log10(x) if x.name == "varDecl" or x.name == "refExpr" or x.name == "intLit" or \
        x.name == "memTo" or x.name == "memFrom" or x.name == "memAlloc" or x.name == "memDelete" or x.name == "addSubInt" or \
        x.name == "addSubFloat" or x.name == "mulFloat" or x.name == "logicalInt" or x.name == "remInt" or x.name == "assFloat" else x)

# the following is not required anymore;
# single['Cluster'].replace(['Seawulf', 'Ookami', 'Exxact', 'Summit'], [0,1,2,3], inplace=True)
# single = df.drop(columns=dr_columns)

print(list(updated_single.columns))
print(len(updated_single))
print(len(list(updated_single.columns)))

# print(updated_single.isnull().sum())
updated_single = updated_single.dropna()
#updated_single = updated_single.drop('addSubFloat', axis=1)
#updated_single = updated_single.drop('mulFloat', axis=1)
#updated_single = updated_single.drop('assFloat', axis=1)

# add the argparser choices to learn over certain runtime brackets here
# this choice will ideally be predicted by the classification module, and internally
# the framework/tool will pass it to this file

if args.bracket == "A":
    print("using kernels runnning for less than 5s")
    updated_single = updated_single[updated_single['runtime'] <= 5e6] 
    
elif args.bracket == "B":
    print("using kernels running for over 5s and less than 100s")
    updated_single = updated_single[(updated_single['runtime'] > 5e6)\
            & (updated_single['runtime'] <= 100e6)] 
    
elif args.bracket == "C":
    print("using kernels running for over 100s")
    updated_single = updated_single[updated_single['runtime'] > 100e6] 
    
elif args.bracket == "neither":
    print("using the entire dataset")
    pass


X = updated_single.iloc[:, 0:-1]
y = updated_single.iloc[:, -1]

print('any null values?', updated_single.isnull().values.any())
print('any NA values?', updated_single.isna().values.any())
#print('any Infinite values?', np.isinf(updated_single).any())

#print(updated_single.isnull().sum())

#print(updated_single.head(5))

train_eval_split=0.8
split_seed=42

#X_train, X_test, y_train, y_test = train_test_split(X, y, train_size=train_eval_split, random_state=split_seed, shuffle=True)

#train_sets = PrepareData(X_train, y_train, train=True)


print("Calling PrepareData\n")
total_sets = PrepareData(X, y, train=True)
print("PrepareData done\n")
m_scaler = total_sets.return_scaler_obj()
print(len(total_sets))
test_split = 0.2

dataset_size = len(total_sets)
indices = list(range(dataset_size))
#print(indices, type(indices))
random.shuffle(indices)
#print(indices, type(indices))
split = int(np.floor(test_split * dataset_size))

train_indices, test_indices = indices[split:], indices[:split]


#val split
val_split=0.1 # 10% of remaining training data
vsplit = int(np.floor(val_split*(1-test_split)*dataset_size))
val_idx, train_idx = indices[split:split+vsplit], indices[split+vsplit:]

train_sampler = SubsetRandomSampler(train_idx)
val_sampler = SubsetRandomSampler(val_idx)
test_sampler = SubsetRandomSampler(test_indices)
tr_loader = DataLoader(total_sets, batch_size=192, sampler=train_sampler)
val_loader = DataLoader(total_sets, batch_size=1, sampler=val_sampler)
te_loader = DataLoader(total_sets, batch_size=1, sampler=test_sampler)
print('train batches: ', len(tr_loader),' validate samples:', len(val_loader),' test samples:', len(te_loader))




from torch.autograd import Variable

#mod = KernelRunModel(69,138).to(device)
#mod = DeepRes(4, Block, 50).to(device)
mod = DeepRes(4, Block, 37).to(device)

def init_weights(m):
    if isinstance(m, nn.Linear):
        #torch.nn.init.kaiming_uniform_(m.weight, nonlinearity='relu')
        torch.nn.init.kaiming_uniform_(m.weight, nonlinearity='relu')
        m.bias.data.fill_(0.01)

mod.apply(init_weights)

#print(mod)

criterion = nn.MSELoss(reduction='mean')
criterion2 = nn.L1Loss(reduction='mean')
criterion3 = nn.L1Loss(reduction='sum')
num_epochs=250
alpha=0.6
lam=1
#opt = torch.optim.Adam(mod.parameters(), lr=1e-2, weight_decay=1e-4)
if args.bracket == "A":
    opt = torch.optim.RMSprop(mod.parameters(), lr=1e-2, momentum=0.87, weight_decay=1e-4)
else:
    opt = torch.optim.Adamax(mod.parameters(), lr=1e-2,  weight_decay=5e-4)
#base_opt = torch.optim.Adam(mod.parameters(), lr=1e-2,  weight_decay=5e-4)
#opt = LARS(optimizer=base_opt, eps=1e-8, trust_coef=0.001)
#opt = LARS(mod.parameters(), lr=1e-2, weight_decay=5e-4)
#opt = torch.optim.Adagrad(mod.parameters(), lr=1e-2, weight_decay=1e-3)

#opt = torch.optim.RMSprop(mod.parameters(), lr=1e-2, momentum=0.87, weight_decay=1e-4)

#opt = torch.optim.Adam(mod.parameters(), lr=1e-3, weight_decay=1e-4)

lr_scheduler = torch.optim.lr_scheduler.CosineAnnealingLR(opt, T_max=num_epochs, eta_min=0)
#lr_scheduler = torch.optim.lr_scheduler.StepLR(opt, step_size=60, gamma=0.1)
#lr_scheduler = torch.optim.lr_scheduler.ExponentialLR(opt, gamma=0.95, verbose=True)
rmse_best=np.inf
mape_best=np.inf
l2l1_best=np.inf
best_model = None
for e in range(num_epochs):
    batch_losses = []
    mod.train()
    for ix, (Xb, yb) in enumerate(tr_loader):
        #opt.zero_grad()
        _X = Variable(Xb).float()
        _y = Variable(yb).float()

        #==========Forward pass===============
        _X = _X.to(device)
        _y = _y.to(device)
       # print(_X.size())
       # print(_y.size())
        preds = mod(_X)
        loss = criterion(preds, _y)
        loss2 = criterion2(preds, _y)
        loss3 = criterion3(preds, _y)
        #total_loss = torch.log(loss + lam*abs(loss2) + alpha*abs(loss3) )
         
        #total_loss = torch.log(loss + lam*loss3) 
        total_loss = torch.log(loss) + loss2
        #total_loss = loss
        #==========backward pass==============

        opt.zero_grad()
        total_loss.backward()
        opt.step()

        batch_losses.append(total_loss.item())
        #all_losses.append(loss.data[0])
    
    mbl = np.mean(np.sqrt(batch_losses)).round(3)
    
    print("Epoch [{}/{}], Batch loss: {}".format(e, num_epochs, mbl))
    
    mod.eval()
    mape=0.0
    rmse=0.0
    l2l1=0.0
    with torch.no_grad():
        gtr_ = list()
        pred_ = list()
        val_loss = list()
        for index, (xt, yt) in enumerate(val_loader):
            gtr_.append(yt.cpu().data.numpy()[0])

            _xt = Variable(xt).float()
            _yt = Variable(yt).float()

            _xt = _xt.to(device)
            _yt = _yt.to(device)

            predictions = mod(_xt)  ## no need to filter out negative op
            loss1 = criterion(predictions, _yt)
            loss2 = criterion2(predictions, _yt)
            val_loss.append(loss1.item()+loss2.item())

            pred_.append(predictions.cpu().data.numpy()[0])
            #print(predictions, _yt)

        mape = mean_absolute_percentage_error(gtr_, pred_)
        rmse = np.sqrt(mean_squared_error(gtr_, pred_))
        l2l1 = np.mean(np.sqrt(val_loss)).round(3)
        print('Epoch:', e, 'RMSE: ', rmse, ' MAPE:', mape, ' L2+L1 loss:', l2l1)
    
    #if rmse < rmse_best:
    #    rmse_best=rmse
    #    mape_best=mape
    #    l2l1_best=l2l1
    #    best_model=copy.deepcopy(mod)
    
    #lr_scheduler.step()

## save trained model
torch.save(mod.state_dict(), 'trained_model.pt')


best_model=copy.deepcopy(mod)

print('\n\nEvaluating Model.......')
print('Best Model - RMSE:', rmse_best, ' MAPE:', mape_best, ' L2+L1-', l2l1_best)
#Evaluate model on testing data
best_model.eval()
less_5_gt = list()
less_5_pred = list()

with torch.no_grad():
    total_loss = 0
    gt_ = list()
    preds_ = list()
    
    # custom prediction metric
    less_5_pr = 0
    less_5_gt = 0
    less_100_pr = 0
    less_100_gt = 0
    more_100_pr = 0
    more_100_gt = 0
    
    for index, (xt, yt) in enumerate(te_loader):
        gt_.append(yt.cpu().data.numpy()[0])
        gr_truth = yt.cpu().data.numpy()[0]


        _xt = Variable(xt).float()
        _yt = Variable(yt).float()
        
        _xt = _xt.to(device)
        _yt = _yt.to(device)
        
        predictions = F.relu(best_model(_xt))
        loss1 = criterion(predictions, _yt)
        preds_.append(predictions.cpu().data.numpy()[0])
        pr_val = predictions.cpu().data.numpy()[0]
        
        if gr_truth <= 5.0:
            less_5_gt += 1
            if abs(gr_truth - pr_val) <= 2.00:
                less_5_pr += 1
        elif gr_truth <= 100.00:
            less_100_gt += 1
            if abs(gr_truth - pr_val) <= 10.00:
                less_100_pr += 1
        else:
            more_100_gt += 1
            if abs(gr_truth-pr_val) <= 0.1*gr_truth:
                more_100_pr +=1

        
        print(predictions.cpu().data.numpy()[0][0],',', _yt.cpu().data.numpy()[0][0])
        total_loss += loss1
    
    mape = mean_absolute_percentage_error(gt_, preds_)
    rmse = np.sqrt(mean_squared_error(gt_, preds_))
    #print('Test Loss: ', np.mean(np.sqrt(total_loss.item())))
    print('RMSE: ', rmse, ' MAPE:', mape)
    print('5: ground truth total- ', less_5_gt, ' predicted total - ', less_5_pr)
    print('100: ground truth total- ', less_100_gt, ' predicted total - ', less_100_pr)
    print(' more 100: ground truth total - ', more_100_gt, ' predicted total - ', more_100_pr)


## Upadte data block when alok uploads qcd data
'''
print('\n\n\nevaluating data from wilson d-slash kernel')
tdf = pd.read_csv(dataset_root+"wilson_exxact_gpu.csv")
tdf2 = pd.read_csv(dataset_root+"wilson_ookami_gpu.csv")
tdf3 = pd.read_csv(dataset_root+"wilson_seawulf_gpu.csv")
tdf4 = pd.read_csv(dataset_root+"wilson_summit_gpu.csv")

single2 = pd.concat([tdf,tdf2,tdf3,tdf4], axis=0)
single2 = single2.drop(columns=dr_columns)


#df2 = pd.read_csv('Wilson.csv')
#single2=df2.drop(columns=dr_columns)
X2 = single2.iloc[:, 0:-1]
y2 = single2.iloc[:, -1]
total_sets2 = PrepareData(X2, y2, train=False, scaler_obj=m_scaler)
test_loader_2 = DataLoader(total_sets2, batch_size=1, shuffle=True)


with torch.no_grad():
    gt2_ = list()
    preds2_ = list()

    for index, (xt, yt) in enumerate(test_loader_2):
        gt2_.append(yt.cpu().data.numpy()[0])
        gr_truth = yt.cpu().data.numpy()[0]


        _xt = Variable(xt).float()
        _yt = Variable(yt).float()

        _xt = _xt.to(device)
        _yt = _yt.to(device)

        predictions = F.relu(best_model(_xt))
        loss1 = criterion(predictions, _yt)
        preds2_.append(predictions.cpu().data.numpy()[0])
        pr_val = predictions.cpu().data.numpy()[0]

        print(predictions.cpu().data.numpy()[0][0],',', _yt.cpu().data.numpy()[0][0])
        

    mape = mean_absolute_percentage_error(gt2_, preds2_)
    rmse = np.sqrt(mean_squared_error(gt2_, preds2_))
    print('RMSE: ', rmse, ' MAPE:', mape)
'''

