#!/usr/bin/env bash

set -u

LEVEL="${LEVEL:-2}"
RESULT_DIR="${RESULT_DIR:-perf_results}"
if (($# > 0)); then
    SIZES=("$@")
else
    SIZES=(5 25 100)
fi
if [[ -n "${KERNEL:-}" ]]; then
    KERNELS=("$KERNEL")
else
    KERNELS=(relu_naive linear_naive matmul_naive softmax_naive conv_naive)
fi

mkdir -p "$RESULT_DIR"

for size in "${SIZES[@]}"; do
    for kernel in "${KERNELS[@]}"; do
        result_file="$RESULT_DIR/${kernel}_level${LEVEL}_size${size}.txt"
        {
            printf 'Kernel: %s\n' "$kernel"
            printf 'Top-down level: %s\n' "$LEVEL"
            printf 'Input size: %s\n\n' "$size"
            printf '$ make %s LEVEL=%s SIZE=%s\n\n' "$kernel" "$LEVEL" "$size"
            make "$kernel" LEVEL="$LEVEL" SIZE="$size"
            status=$?
            printf '\nExit status: %s\n' "$status"
        } > "$result_file" 2>&1

        if grep -q '^Exit status: 0$' "$result_file"; then
            printf 'PASS  %s size=%s -> %s\n' "$kernel" "$size" "$result_file"
        else
            printf 'FAIL  %s size=%s -> %s\n' "$kernel" "$size" "$result_file"
        fi
    done
done

printf '\nResults saved in %s/\n' "$RESULT_DIR"
