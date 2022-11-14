# C++ compiler
CXX = clang++
CFLAGS = -Wall -O3 -fopenmp
OFFLOAD_FLAGS = -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70

all:
	@echo "Please run make for individual archs"
	@echo "make -f Makefile.<arch>"

data: test
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=100 -c gauss.cpp -o gauss_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=100 -c gauss_kernel.cpp -o gauss_kernel_data_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=100 gauss_kernel_data_test.o gauss_test.o -o gauss_data_test.out; \
	$(RM) gauss_kernel_data_test.o gauss_test.o;

test:
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=100 -c gauss.cpp -o gauss_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=100 -c gauss_kernel.cpp -o gauss_kernel_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=100 gauss_kernel_test.o gauss_test.o -o gauss_test.out; \
	$(RM) gauss_kernel_test.o gauss_test.o;

clean:
	$(RM) *.o *.out
