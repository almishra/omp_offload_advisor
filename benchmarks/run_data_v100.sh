for i in `ls -d */ | grep -v omp_gpu`
do 
  cd $i;
  make -f Makefile.v100 data > makefile.log
  for i in `ls *_v100.out`
  do 
    output=$(./$i); 
    kernel=$(echo $output | sed 's/_test_v100.csv//g' | sed 's/output_//g');
    echo -ne "${kernel},"; 
    cat $output | awk -F',' '{size+=$3;time+=$4;}END{printf "%ld,%ld\n",size, time}';
  done
  cd ..
done
