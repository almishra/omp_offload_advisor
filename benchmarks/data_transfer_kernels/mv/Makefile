# C++ compiler
CXX = clang++
CFLAGS = -Wall -O3 -fopenmp
OFFLOAD_FLAGS = -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70
N1=100000
N2=10000

all:
	@echo "Please run make for individual archs"
	@echo "make -f Makefile.<arch>"

data: test mv.cpp mv_kernel_data.cpp
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN1=$(N1) -DN2=$(N2) -c mv.cpp -o mv_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN1=$(N1) -DN2=$(N2) -c mv_kernel_data.cpp -o mv_kernel_data_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN1=$(N1) -DN2=$(N2) mv_kernel_data_test.o  mv_test.o -o mv_data_test.out
	$(RM) mv_*_test.o

test: mv.cpp mv_kernel.cpp
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN1=$(N1) -DN2=$(N2) -c mv.cpp -o mv_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN1=$(N1) -DN2=$(N2) -c mv_kernel.cpp -o mv_kernel_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN1=$(N1) -DN2=$(N2) mv_kernel_test.o  mv_test.o -o mv_test.out
	$(RM) mv_*_test.o

clean:
	$(RM) *.o *.out
