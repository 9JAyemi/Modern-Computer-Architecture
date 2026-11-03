#!/usr/bin/env bash
# Collects Top-down (toplev.py) and wall-clock data for the matmul variants.
# Must run on the ece5755 node (perf counters are disabled on the login node):
#   srun -p ece5755 -n 1 --mem=2g --time=00:05:00 ./scripts/run_perf.sh toplev 256 1024
#   srun -p ece5755 -n 1 --mem=2g --time=00:05:00 ./scripts/run_perf.sh time 256 1024 2048
# OPT (default -O0, same as the Makefile/Lab 1) selects the optimization level.
# Every run is pinned to CPU 0 with taskset and toplev only reports core S0-C0,
# i.e. exactly the core the program runs on.
set -u
cd "$(dirname "$0")/.."
MODE="$1"; shift
OPT="${OPT:--O0}"
LEVEL="${LEVEL:-2}"
OUT=perf_results
BIN="bin_perf/run${OPT}"
TOPLEV=/classes/ece5755/pmu-tools/toplev.py
mkdir -p "$OUT" "$BIN"

SRC="./utils/data_utils.c ./kernel/conv.c ./kernel/functional.c ./kernel/matrix_ops.c ./kernel/linear.c ./kernel/nn.c ./kernel/attention.c"
FLAGS="-I. -lm -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5 $OPT"

# name : source : extra flags
VARIANTS=(
  "naive:perf/matmul_naive.c:"
  "tiled_ijk_64x256x32:perf/matmul_blocking_ijk.c:-DBLOCK_SIZE_I=64 -DBLOCK_SIZE_J=256 -DBLOCK_SIZE_K=32"
  "tiled_ikj_16x64x16:perf/matmul_blocking.c:-DBLOCK_SIZE_I=16 -DBLOCK_SIZE_J=64 -DBLOCK_SIZE_K=16"
  "tiled_ikj_64x256x32:perf/matmul_blocking.c:-DBLOCK_SIZE_I=64 -DBLOCK_SIZE_J=256 -DBLOCK_SIZE_K=32"
)

# Repetitions per size so the matmul call dominates program runtime
reps_for() {
  case "$1" in
    256) echo 100 ;;
    512) echo 20 ;;
    1024) echo 3 ;;
    *) echo 1 ;;
  esac
}

for v in "${VARIANTS[@]}"; do
  IFS=: read -r name src extra <<< "$v"
  gcc -o "$BIN/$name" "$src" $SRC $FLAGS $extra 2>/dev/null || { echo "build failed: $name"; exit 1; }
done

for size in "$@"; do
  reps=$(reps_for "$size")
  for v in "${VARIANTS[@]}"; do
    IFS=: read -r name _ _ <<< "$v"
    tag="${name}_size${size}${OPT}"
    if [ "$MODE" = toplev ]; then
      python3 "$TOPLEV" --core S0-C0 -l"$LEVEL" -v --no-desc --force-cpu spr \
          taskset -c 0 "$BIN/$name" "$size" "$reps" > "$OUT/toplev_${tag}.txt" 2>&1
      echo "toplev $tag reps=$reps -> $OUT/toplev_${tag}.txt"
    else
      { /usr/bin/time -f "real %e user %U sys %S" taskset -c 0 "$BIN/$name" "$size" "$reps" >/dev/null; } \
          2> "$OUT/time_${tag}.txt"
      echo "time $tag reps=$reps $(cat "$OUT/time_${tag}.txt")"
    fi
  done
done
