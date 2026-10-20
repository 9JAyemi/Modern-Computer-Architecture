#!/usr/bin/env bash
# Tile-size sweep for matmul_blocking. Builds one binary per (BI, BJ, BK)
# configuration and times a single call at the given size on CPU 0.
# Run on the ece5755 node, e.g.:
#   srun -p ece5755 -n 1 --mem=2g --time=00:05:00 ./scripts/sweep_tiles.sh 1024
set -eu
cd "$(dirname "$0")/.."
SIZE="${1:-1024}"
OPT="${OPT:--O0}"
BIN=bin_perf/sweep${OPT}
mkdir -p "$BIN"
SRC="./utils/data_utils.c ./kernel/conv.c ./kernel/functional.c ./kernel/matrix_ops.c ./kernel/linear.c ./kernel/nn.c ./kernel/attention.c"
FLAGS="-I. -lm -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5"

for bi in ${BIS:-16 64 256}; do
  for bj in ${BJS:-64 256 1024}; do
    for bk in ${BKS:-16 64 256}; do
      exe="$BIN/blk_${bi}_${bj}_${bk}"
      [ -x "$exe" ] || gcc -o "$exe" perf/matmul_blocking.c $SRC $FLAGS $OPT \
          -DBLOCK_SIZE_I=$bi -DBLOCK_SIZE_J=$bj -DBLOCK_SIZE_K=$bk
      t=$( { /usr/bin/time -f "%e" taskset -c 0 "$exe" "$SIZE" 1 >/dev/null; } 2>&1 )
      echo "$OPT $SIZE $bi $bj $bk $t"
    done
  done
done
