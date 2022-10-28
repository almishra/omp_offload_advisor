#for i in `ls | grep -v omp_gpu | grep -v .sh | grep -v .csv` 
for i in `ls -d */ | grep -v om_gpu`
do 
  cd $i;
  echo -ne "$i,";
  make data > /dev/null
  ./run_data.sh > /dev/null;
  mult=$(cat 1.csv | grep -A1 "\"Start\"" | tail -n1 | awk -F',' '{print $2}'); 
  mu=1000;
  if [[ "$mult" == "ms" ]]
  then
    mu=1;
  elif [[ "$mult" == "us" ]]
  then
    mu=0.001
  fi
  cat 1.csv | grep "CUDA memcpy " | awk -v m=${mu} -F',' '{time+=($2*m); size+=$12} END{printf "%.4f,%.4f,%d\n", size,time,m}'
  echo -ne "${i}_data,";
  
  mult=$(cat 2.csv | grep -A1 "\"Start\"" | tail -n1 | awk -F',' '{print $2}'); 
  mu=1000;
  if [[ "$mult" == "ms" ]]
  then
    mu=1;
  elif [[ "$mult" == "us" ]]
  then
    mu=0.001
  fi
  cat 2.csv | grep "CUDA memcpy " | awk -v m=${mu} -F',' '{time+=($2*m); size+=$12} END{printf "%.4f,%.4f,%d\n", size,time,m}'

  cd ..
done
