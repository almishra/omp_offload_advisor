(nvprof --print-gpu-trace --csv ./mm_test.out 2>&1) > 1.csv
(nvprof --print-gpu-trace --csv ./mm_data_test.out 2>&1) > 2.csv
