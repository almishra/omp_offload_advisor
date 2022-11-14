# C++ compiler
CXX = clang++
CFLAGS = -Wall -O3 -fopenmp -DDEBUG
OFFLOAD_FLAGS = -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70
N1=10000000

all:
	@echo "Please run make for individual archs"
	@echo "make -f Makefile.<arch>"

data: cc_test.out cc_data_test.out cc.cpp cc_kernel_data.cpp
cc_data_test.out:
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=$(N1) -c cc.cpp -o cc_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=$(N1) -c cc_kernel_data.cpp -o cc_kernel_data_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=$(N1) -lm cc_kernel_data_test.o cc_test.o -o cc_data_test.out
	$(RM) cc*_test.o

test: cc_test.out
cc_test.out: cc.cpp cc_kernel.cpp cc.cpp cc_kernel.cpp
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=$(N1) -c cc.cpp -o cc_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=$(N1) -c cc_kernel.cpp -o cc_kernel_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=$(N1) -lm cc_kernel_test.o cc_test.o -o cc_test.out
	$(RM) cc*_test.o

clean:
	$(RM) *.o *.out
