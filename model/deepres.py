import torch
import torch.nn as nn
#import torch.nn.functional as F

class Block(nn.Module):
    def __init__(self, ip_features):
        super(Block, self).__init__()
        self.l1 = nn.Linear(ip_features, ip_features)
        self.bn1 = nn.BatchNorm1d(num_features=ip_features)
        self.relu1 = nn.ReLU()
        self.l2 = nn.Linear(ip_features, ip_features)
        self.bn2 = nn.BatchNorm1d(num_features=ip_features)
        self.relu2 = nn.ReLU()
        self.l3 = nn.Linear(ip_features, ip_features)
        self.bn3 = nn.BatchNorm1d(num_features=ip_features)
        self.relu3 = nn.ReLU()
        
    def forward(self, x):
        identity = x
        
        x = self.l1(x)
        x = self.bn1(x)
        x = self.relu1(x)
        
        x = self.l2(x)
        x = self.bn2(x)
        x = self.relu2(x)
        
        x = self.l3(x)
        x = self.bn3(x)
        x = self.relu3(x)
        
        x += identity
        
        return x


class DeepRes(nn.Module):
    def __init__(self, num_blocks, block, ip_features):
        super(DeepRes, self).__init__()
        self.l1 = nn.Linear(ip_features, ip_features)
        self.relu1 = nn.ReLU()
        
        self.block1 = self.make_layers_(block, ip_features)

        self.lb2 = nn.Linear(ip_features, ip_features*2)
        self.relu2 = nn.ReLU()
        
        self.block2 = self.make_layers_(block, ip_features*2)
        
        self.lb3 = nn.Linear(ip_features*2, ip_features*4)
        self.relu3 = nn.ReLU()
        
        self.block3 = self.make_layers_(block, ip_features*4)
        
        self.lb4 = nn.Linear(ip_features*4, ip_features*2)
        self.relu4 = nn.ReLU()

        self.block4 = self.make_layers_(block, ip_features*2)
        
        self.dr = nn.Dropout(p=0.2)
        self.op = nn.Linear(ip_features*2, 1)
        self.relu5 = nn.ReLU()    
    
    def forward(self, x):
        x = self.relu1(self.l1(x))
        x = self.block1(x)
        #identity = x
        
        x = self.relu2(self.lb2(x))
        x = self.block2(x)
        identity = x

        x = self.relu3(self.lb3(x))
        x = self.block3(x)
        #x = self.dr(x)
        
        x = self.relu4(self.lb4(x))
        x = self.block4(x)
        
        x += identity
        
        #x = self.dr(x)
        #x = self.relu5(self.op(x))
        x = self.op(x)
        return x
        
    def make_layers_(self, block, num_features):
        layers = block(num_features)
        return nn.Sequential(layers)
