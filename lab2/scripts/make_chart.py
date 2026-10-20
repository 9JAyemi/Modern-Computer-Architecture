"""Builds the Top-down bar chart and runtime table for the Lab 2 report.

Reads perf_results/toplev_*.txt and perf_results/time_*.txt (produced by
scripts/run_perf.sh) and writes report/topdown.pdf and report/runtime_table.tex.
Usage: python3 scripts/make_chart.py
"""
import os
import re

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
RES = os.path.join(ROOT, "perf_results")
OUT = os.path.join(ROOT, "report")
os.makedirs(OUT, exist_ok=True)

VARIANTS = [
    ("naive", "matmul (Lab 1, ijk)"),
    ("tiled_ijk_64x256x32", "tiled ijk 64x256x32"),
    ("tiled_ikj_16x64x16", "tiled ikj 16x64x16"),
    ("tiled_ikj_64x256x32", "tiled ikj 64x256x32 (final)"),
]
REPS = {256: 100, 512: 20, 1024: 3, 2048: 1}

# Segment order follows the Top-down hierarchy; colors are the first five
# categorical slots of the reference palette, in fixed order.
SEGMENTS = [
    ("Frontend_Bound", "Frontend bound", "#2a78d6"),
    ("Bad_Speculation", "Bad speculation", "#eb6834"),
    ("Backend_Bound.Memory_Bound", "Backend: memory bound", "#1baf7a"),
    ("Backend_Bound.Core_Bound", "Backend: core bound", "#eda100"),
    ("Retiring", "Retiring", "#e87ba4"),
]


def read_toplev(name, size, opt):
    path = os.path.join(RES, f"toplev_{name}_size{size}{opt}.txt")
    if not os.path.exists(path):
        return None
    vals = {}
    for line in open(path):
        m = re.match(r"^\S+\s+(\S+)\s+% Slots\s+(-?[\d.]+)", line)
        if m:
            vals[m.group(1)] = max(0.0, float(m.group(2)))
    # Normalize the five plotted segments to 100% (toplev rounding/multiplexing
    # leaves sums of ~99-101%)
    seg = [vals.get(k, 0.0) for k, _, _ in SEGMENTS]
    total = sum(seg)
    return [100 * v / total for v in seg], vals


def read_time(name, size, opt):
    path = os.path.join(RES, f"time_{name}_size{size}{opt}.txt")
    if not os.path.exists(path):
        return None
    m = re.search(r"real ([\d.]+)", open(path).read())
    return float(m.group(1)) if m else None


def chart(groups, fname):
    rows = []  # (label, segments) top to bottom, with None as a group gap
    for size, opt in groups:
        for name, label in VARIANTS:
            r = read_toplev(name, size, opt)
            if r:
                rows.append((f"{size}$^2$ {opt}  {label}", r[0]))
        rows.append(None)
    rows = rows[:-1]

    fig, ax = plt.subplots(figsize=(7.2, 0.24 * len(rows) + 0.9))
    y = 0
    yticks, ylabels = [], []
    for row in rows:
        if row is None:
            y += 0.6
            continue
        label, seg = row
        left = 0.0
        for (key, _, color), v in zip(SEGMENTS, seg):
            ax.barh(y, v, left=left, height=0.78, color=color,
                    edgecolor="white", linewidth=1.0)
            left += v
        # Direct label on the Retiring segment only
        ax.text(99, y, f"{seg[-1]:.0f}%", va="center", ha="right",
                fontsize=6.5, color="#0b0b0b")
        yticks.append(y)
        ylabels.append(label)
        y += 1
    ax.set_yticks(yticks)
    ax.set_yticklabels(ylabels, fontsize=6.8)
    ax.invert_yaxis()
    ax.set_xlim(0, 100)
    ax.set_xlabel("% of pipeline slots (toplev level 1, backend split at level 2)",
                  fontsize=7, color="#52514e")
    ax.tick_params(axis="x", labelsize=6.5, colors="#52514e")
    ax.tick_params(axis="y", length=0)
    for s in ("top", "right", "left"):
        ax.spines[s].set_visible(False)
    ax.spines["bottom"].set_color("#b5b4ad")
    handles = [plt.Rectangle((0, 0), 1, 1, color=c) for _, _, c in SEGMENTS]
    ax.legend(handles, [n for _, n, _ in SEGMENTS], ncol=5, fontsize=6.3,
              loc="lower center", bbox_to_anchor=(0.4, 1.0), frameon=False,
              handlelength=1.0, columnspacing=1.0)
    fig.tight_layout()
    fig.savefig(os.path.join(OUT, fname))
    fig.savefig(os.path.join(OUT, fname.replace(".pdf", ".png")), dpi=160)


def runtime_table():
    lines = []
    for size in (256, 1024, 2048):
        base = {opt: read_time("naive", size, opt) for opt in ("-O0", "-O2")}
        for name, label in VARIANTS:
            cells = []
            for opt in ("-O0", "-O2"):
                t = read_time(name, size, opt)
                if t is None:
                    cells += ["--", "--"]
                    continue
                per = t / REPS[size]
                sp = base[opt] / t if base[opt] else float("nan")
                cells += [f"{t:.2f} ({per:.3f})", f"{sp:.2f}$\\times$"]
            lines.append(f"{size} & {label} & " + " & ".join(cells) + r" \\")
        lines.append(r"\midrule")
    lines = lines[:-1]
    open(os.path.join(OUT, "runtime_table.tex"), "w").write("\n".join(lines) + "\n")


if __name__ == "__main__":
    chart([(256, "-O0"), (1024, "-O0"), (2048, "-O0"), (2048, "-O2")], "topdown.pdf")
    runtime_table()
    print(open(os.path.join(OUT, "runtime_table.tex")).read())
