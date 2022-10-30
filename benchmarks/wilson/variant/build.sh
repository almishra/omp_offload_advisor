if [ "$#" != 1 ]
then
  echo "Please provide file"
  exit -1;
else
  FILE=$1
fi

GPU_FLAGS="-fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=gfx906"
FILE_NAME=${FILE%.*}
echo $FILE_NAME
VARDIR=$(pwd)/${FILE_NAME}_variants
DIR=$(pwd)
echo "-- " $DIR
cd ${DIR}
CUR=$(pwd)

for X in 16 32 48 64; do
for Y in 16 32 48 64; do
for Z in 16 32 48 64; do
for T in 16 32 48 64; do
  cd ${CUR}
  OUTDIR=${DIR}/${X}_${Y}_${Z}_${T}
  if [ ! -d "${OUTDIR}" ];
  then
    echo "Creating directory ${OUTDIR}"
    mkdir ${OUTDIR}
  else
    echo "Binary directory ${OUTDIR} already exists";
  fi

  VARIANT_DIR=${VARDIR}/${X}_${Y}_${Z}_${T}
  echo ${VARIANT_DIR}
  if [ ! -d "${VARIANT_DIR}" ] 
  then
    echo "Skipping ${VARIANT_DIR}";
    continue;
  fi 

  clang++ -fopenmp ${GPU_FLAGS} -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -c wilson.cpp -o wilson.o 
  cd ${VARIANT_DIR}
  for i in `ls *.cpp`; do
    OUT=${i%.*}.o
    if [ ! -f ${OUTDIR}/${OUT}ut ]
    then
      echo "clang++ -fopenmp ${GPU_FLAGS} -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -c $i -o ${OUT};"
      clang++ -fopenmp ${GPU_FLAGS} -DLX=${X} -DLY=${Y} -DLZ=${Z} -DLT=${T} -c $i -o ${OUT};
      echo "clang++ -fopenmp ${GPU_FLAGS} ${OUT} ${DIR}/wilson.o -o ${OUTDIR}/${OUT}ut;"
      clang++ -fopenmp ${GPU_FLAGS} ${OUT} ${DIR}/wilson.o -o ${OUTDIR}/${OUT}ut;
      rm ${OUT}; 
    fi
  done
  rm ${DIR}/wilson.o;
done;done;done;done
