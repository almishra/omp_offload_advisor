if [ "$#" != 1 ]
then
  DIR=llvm/opt/llvm
else
  DIR=$1/opt/llvm
fi

echo "${DIR}/bin/clang -fopenmp -Xclang -load -Xclang ${DIR}/lib/omp-advisor.so -Xclang -plugin -Xclang omp-advisor test.cpp -c"
$DIR/bin/clang -fopenmp -Xclang -load -Xclang $DIR/lib/omp-advisor.so -Xclang -plugin -Xclang omp-advisor test.cpp -c

echo
echo
echo "${DIR}/bin/clang -Xclang -load -Xclang ${DIR}/lib/omp-advisor.so -Xclang -plugin -Xclang omp-advisor -Xclang -plugin-arg-omp-advisor -Xclang help test.cpp -c"
$DIR/bin/clang -Xclang -load -Xclang $DIR/lib/omp-advisor.so -Xclang -plugin -Xclang omp-advisor -Xclang -plugin-arg-omp-advisor -Xclang help test.cpp -c
