#(nvprof --print-gpu-trace --csv ./proxy_app_test.out 2>&1) > 1.csv
#(nvprof --print-gpu-trace --csv ./proxy_app_data_test.out 2>&1) > 2.csv
(nvprof --print-gpu-trace --csv ./proxy_app_test.out 2>&1) > 1.csv
(nvprof --print-gpu-trace --csv ./proxy_app_data_test.out 2>&1) > 2.csv
