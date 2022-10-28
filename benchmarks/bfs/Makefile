# C++ compiler
CXX = clang++
CFLAGS = -Wall -O3 -fopenmp
OFFLOAD_FLAGS = -fopenmp-targets=nvptx64 -Xopenmp-target -march=sm_70

all:
	@echo "Please run make for individual archs"
	@echo "make -f Makefile.<arch>"

data: test
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 -c bfs.cpp -o bfs_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 -c bfs_kernel1_data.cpp -o bfs_kernel1_data_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 -c bfs_kernel2_data.cpp -o bfs_kernel2_data_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 bfs_kernel1_data_test.o bfs_kernel2_data_test.o bfs_test.o -o bfs_data_test.out; \
	$(RM) bfs*_test.o;

test:
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 -c bfs.cpp -o bfs_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 -c bfs_kernel1.cpp -o bfs_kernel1_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 -c bfs_kernel2.cpp -o bfs_kernel2_test.o; \
	$(CXX) $(CFLAGS) $(OFFLOAD_FLAGS) -DN=1000000 bfs_kernel1_test.o bfs_kernel2_test.o bfs_test.o -o bfs_test.out; \
	$(RM) bfs*_test.o;

clean:
	$(RM) *.o *.out
