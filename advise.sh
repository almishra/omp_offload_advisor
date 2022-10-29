if [ "$#" != 1 ]
then
  FILE=test.cpp
else
  FILE=$1
fi

ADVISOR_PATH=$(pwd)
FILE_NAME=${FILE%.*}
echo $FILE_NAME
VARDIR=$(pwd)/${FILE_NAME}_variants
DIR=$(pwd)/${FILE%/*}
echo "-- " $DIR
cd ${DIR}
CUR=$(pwd)

for X in 16 32 48 64; do
for Y in 16 32 48 64; do
for Z in 16 32 48 64; do
for T in 16 32 48 64; do
  OUTDIR=${DIR}/${X}_${Y}_${Z}_${T}
  [ ! -d "${OUTDIR}" ] && mkdir ${OUTDIR}

  VARIANT_DIR=${VARDIR}/${X}_${Y}_${Z}_${T}
  [ ! -d "${VARIANT_DIR}" ] && mkdir ${VARIANT_DIR}

  echo "${ADVISOR_PATH}/llvm/opt/llvm/bin/clang++ -Wall -O3 -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -fopenmp -Xclang -load -Xclang ${ADVISOR_PATH}/llvm/opt/llvm/lib/omp-advisor.so -Xclang -plugin -Xclang omp-advisor ${FILE} -c"
  ${ADVISOR_PATH}/llvm/opt/llvm/bin/clang++ -Wall -O3 -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -fopenmp -Xclang -load -Xclang ${ADVISOR_PATH}/llvm/opt/llvm/lib/omp-advisor.so -Xclang -plugin -Xclang omp-advisor ${ADVISOR_PATH}/${FILE} -c
  cp *.h ${VARIANT_DIR}
  mv ${VARDIR}/*.cpp ${VARIANT_DIR}/
#  clang++ -fopenmp -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70 -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -c wilson.cpp -o wilson.o 
#  cd ${VARIANT_DIR}
#  pwd
#  for i in `ls *.cpp`; do
#    OUT=${i%.*}.o
#    echo "clang++ -fopenmp -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70 -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -c $i -o ${OUT};"
#    clang++ -fopenmp -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70 -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -c $i -o ${OUT};
#    echo "clang++ -fopenmp -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70 ${OUT} ${DIR}/wilson.o -o ${OUTDIR}/${OUT}ut;"
#    clang++ -fopenmp -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70 ${OUT} ${DIR}/wilson.o -o ${OUTDIR}/${OUT}ut;
#    rm ${OUT}; 
#  done
#  rm ${DIR}/wilson.o;
  cd ${CUR}
done;done;done;done
