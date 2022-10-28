(nvprof --print-gpu-trace --csv ./bfs_test.out 2>&1) > 1.csv
(nvprof --print-gpu-trace --csv ./bfs_data_test.out 2>&1) > 2.csv
