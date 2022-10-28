#for i in `ls | grep -v omp_gpu | grep -v .sh | grep -v .csv` 
for i in `ls -d */ | grep -v omp_gpu`
do 
  cd $i;
  make -f Makefile.a100 data > makefile_a100.log
  for i in `ls *_a100.out`
  do 
    output=$(./$i 2> /dev/null); 
    kernel=$(echo $output | sed 's/_test_a100.csv//g' | sed 's/output_//g');
    echo -ne "${kernel},"; 
    cat $output | awk -F',' '{size+=$3;time+=$4;}END{printf "%ld,%ld\n",size, time}';
  done
#  ./run_data_a100.sh > makefile_a100.log
#  mult=$(cat 1_a100.csv | grep -A1 "\"Start\"" | tail -n1 | awk -F',' '{print $2}'); 
#  mu=1000;
#  if [[ "$mult" == "ms" ]]
#  then
#    mu=1;
#  elif [[ "$mult" == "us" ]]
#  then
#    mu=0.001
#  fi
#  cat 1_a100.csv | grep "CUDA memcpy " | awk -v m=${mu} -F',' '{time+=($2*m); size+=$12} END{printf "%.4f,%.4f,%d\n", size,time,m}'
#  echo -ne "${i}_data,";
#  
#  mult=$(cat 2_a100.csv | grep -A1 "\"Start\"" | tail -n1 | awk -F',' '{print $2}'); 
#  mu=1000;
#  if [[ "$mult" == "ms" ]]
#  then
#    mu=1;
#  elif [[ "$mult" == "us" ]]
#  then
#    mu=0.001
#  fi
#  cat 2_a100.csv | grep "CUDA memcpy " | awk -v m=${mu} -F',' '{time+=($2*m); size+=$12} END{printf "%.4f,%.4f,%d\n", size,time,m}'

  cd ..
done
