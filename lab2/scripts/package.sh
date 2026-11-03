#!/usr/bin/env bash
# Builds lab2.zip (code + scripts + raw results) in the parent directory.
set -eu
cd "$(dirname "$0")/../.."
rm -f lab2.zip
zip -rq lab2.zip lab2 \
    -x 'lab2/bin_perf/*' 'lab2/lab2' 'lab2/tests/all_tests' 'lab2/matmul_naive' \
       'lab2/matmul_blocking' 'lab2/matmul_blocking_ijk' 'lab2/*.pdf' 'lab2/report/*.aux' \
       'lab2/report/*.log' 'lab2/mnist_data/*' '*/.DS_Store' '*/__pycache__/*'
unzip -l lab2.zip | tail -1
