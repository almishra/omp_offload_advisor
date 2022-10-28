module unload clang                                                             
module load clang/12.0.0_k80
module unload cuda113
module load cuda10.1/toolkit/10.1.243

for i in `ls -d */ | grep -v omp_gpu`
do 
  cd $i;
  make -f Makefile.k80 data > makefile_k80.log
  for i in `ls *_k80.out`
  do 
    output=$(./$i); 
    kernel=$(echo $output | sed 's/_test_k80.csv//g' | sed 's/output_//g');
    echo -ne "${kernel},"; 
    cat $output | awk -F',' '{size+=$3;time+=$4;}END{printf "%ld,%ld\n",size, time}';
  done
  cd ..
done
