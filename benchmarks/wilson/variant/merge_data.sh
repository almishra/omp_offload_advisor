CUR=$(pwd)
for i in 16 32 48 64; do 
for j in 16 32 48 64; do 
for k in 16 32 48 64; do 
for l in 16 32 48 64; do 
  cd ${CUR}; 
  cd ${i}_${j}_${k}_${l}; 
  for out in `ls output*.csv 2>/dev/null`; do 
    echo -ne "${i}_${j}_${k}_${l}," >> ${CUR}/1.csv
    cat ${out} >> ${CUR}/1.csv;
  done; 
done; done; done; done
