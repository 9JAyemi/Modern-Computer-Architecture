#!/usr/bin/env bash
# Builds lab2.zip (code + scripts + report sources + raw results) in the parent directory.
set -eu
cd "$(dirname "$0")/../.."
python3 - <<'PY'
import os, zipfile
skip_dirs = {"bin_perf", "mnist_data", "__pycache__"}
skip_files = {"lab2", "matmul_naive", "matmul_blocking", "matmul_blocking_ijk",
              "all_tests", ".DS_Store", "lab2_fall2026.pdf"}
with zipfile.ZipFile("lab2.zip", "w", zipfile.ZIP_DEFLATED) as z:
    n = 0
    for root, dirs, files in os.walk("lab2"):
        dirs[:] = sorted(d for d in dirs if d not in skip_dirs)
        for f in sorted(files):
            if f in skip_files or f.endswith((".aux", ".log", ".xdv")):
                continue
            z.write(os.path.join(root, f)); n += 1
print(f"lab2.zip: {n} files")
PY
