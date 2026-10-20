# Assignment 2: Optimize the performance of the ML operations using tiling and blocking

## Code
- `kernel/matrix_ops.c`
  - `matmul_blocking()` - tiled matmul (tiles `BLOCK_SIZE_I x BLOCK_SIZE_J x BLOCK_SIZE_K`,
    default 64 x 256 x 32, i-k-j order inside each tile). Tile sizes can be overridden
    with `-DBLOCK_SIZE_I=...` etc.
  - `matmul_quant()` - A packs four 8-bit values per `unsigned int`, most significant
    byte first (`0x01020304` = `[1, 2, 3, 4]`).
- `kernel/attention.c` - `scaled_dot_product_attention()` = softmax(QK^T / sqrt(depth)) V.

## Tests
    make test

## Profiling (must run on the ece5755 node; perf counters are disabled on the login node)
    srun -p ece5755 -n 1 --mem=2g --time=00:05:00 --pty bash
    make matmul_naive SIZE=1024 REPS=3 LEVEL=2
    make matmul_blocking SIZE=1024 REPS=3 LEVEL=2 BI=64 BJ=256 BK=32
    make matmul_blocking_ijk SIZE=1024 REPS=3 LEVEL=2

Each target pins the program to CPU 0 (`taskset -c 0`) and toplev reports only that core
(`--core S0-C0`). The program loops over the matmul `REPS` times so it dominates runtime.

Full data collection used for the report:

    ./scripts/run_perf.sh toplev 256 1024     # inside srun, likewise for 2048
    ./scripts/run_perf.sh time 256 1024 2048
    OPT=-O2 ./scripts/run_perf.sh time 256 1024 2048
    ./scripts/sweep_tiles.sh 1024              # tile-size sweep
    python3 scripts/make_chart.py              # chart + runtime table

Raw results are in `perf_results/`.
