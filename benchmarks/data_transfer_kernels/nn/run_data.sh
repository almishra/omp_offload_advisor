(nvprof --print-gpu-trace --csv ./nn_test.out 2>&1) > 1.csv
(nvprof --print-gpu-trace --csv ./nn_data_test.out 2>&1) > 2.csv
