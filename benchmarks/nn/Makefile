# C++ compiler
CXX = clang++
CFLAGS = -Wall -O3 -fopenmp
OFFLOAD_FLAGS = -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70
N=10000000

all:
	@echo "Please run make for individual archs"
	@echo "make -f Makefile.<arch>"

data: test nn.cpp nn_kernel_data.cpp
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DREC_WINDOW=$(N) -c nn.cpp -o nn_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DREC_WINDOW=$(N) -c nn_kernel_data.cpp -o nn_kernel_data_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DREC_WINDOW=$(N) -lm nn_kernel_data_test.o nn_test.o -o nn_data_test.out
	$(RM) *_test.o

test: nn.cpp nn_kernel.cpp
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DREC_WINDOW=$(N) -c nn.cpp -o nn_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DREC_WINDOW=$(N) -c nn_kernel.cpp -o nn_kernel_test.o
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DREC_WINDOW=$(N) -lm nn_kernel_test.o nn_test.o -o nn_test.out
	$(RM) *_test.o

clean:
	$(RM) *.out *.o
