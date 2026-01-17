"""
Generate a bar chart from a single CHEHAB CSV result set.

Arguments:
  --metric {exec,compile,noise}         Which metric to plot.
  --csv                                 Path to the CHEHAB CSV file.
  --label                               Legend label for the series (default: CHEHAB).
  --timeout                             Value used to cap/mark timeouts (default: 7200).
  --output                              Output image path (default: graph.png).
  --show                                Also display the plot interactively.

Example:
  python generate_graphs.py --metric exec --csv results_RL.csv --label "CHEHAB RL" --output exec_time.png
"""

import argparse
import csv
import math
from pathlib import Path
from typing import Dict, List, Tuple

import matplotlib.pyplot as plt
import numpy as np

# Benchmarks are plotted in this order.
BENCHMARKS = [
    "Box Blur 3 Į- 3",
    "Box Blur 4 Į- 4",
    "Box Blur 5 Į- 5",
    "Dot Product 4",
    "Dot Product 8",
    "Dot Product 16",
    "Dot Product 32",
    "Hamm. Dist. 4",
    "Hamm. Dist. 8",
    "Hamm. Dist. 16",
    "Hamm. Dist. 32",
    "L2 Distance 4",
    "L2 Distance 8",
    "L2 Distance 16",
    "L2 Distance 32",
    "Linear Reg. 4",
    "Linear Reg. 8",
    "Linear Reg. 16",
    "Linear Reg. 32",
    "Poly. Reg. 4",
    "Poly. Reg. 8",
    "Poly. Reg. 16",
    "Poly. Reg. 32",
    "Gx 3 Į- 3",
    "Gx 4 Į- 4",
    "Gx 5 Į- 5",
    "Gy 3 Į- 3",
    "Gy 4 Į- 4",
    "Gy 5 Į- 5",
    "Rob. Cross 3 Į- 3",
    "Rob. Cross 4 Į- 4",
    "Rob. Cross 5 Į- 5",
    "Mat. Mul. 3 Į- 3",
    "Mat. Mul. 4 Į- 4",
    "Mat. Mul. 5 Į- 5",
    "Max 3",
    "Max 4",
    "Max 5",
    "Sort 3",
    "Sort 4",
    "Tree 50-50-5",
    "Tree 50-50-10",
    "Tree 100-50-5",
    "Tree 100-50-10",
    "Tree 100-100-5",
    "Tree 100-100-10",
]

# Map the plot labels to the corresponding `benchmark` value in CHEHAB CSVs.
CHEHAB_MAP = {
    "Box Blur 3 Į- 3": "box_blur_3",
    "Box Blur 4 Į- 4": "box_blur_4",
    "Box Blur 5 Į- 5": "box_blur_5",
    "Dot Product 3": "dot_product_3",
    "Dot Product 4": "dot_product_4",
    "Dot Product 5": "dot_product_5",
    "Dot Product 8": "dot_product_8",
    "Dot Product 16": "dot_product_16",
    "Dot Product 25": "dot_product_25",
    "Dot Product 32": "dot_product_32",
    "Hamm. Dist. 3": "hamming_dist_3",
    "Hamm. Dist. 4": "hamming_dist_4",
    "Hamm. Dist. 5": "hamming_dist_5",
    "Hamm. Dist. 8": "hamming_dist_8",
    "Hamm. Dist. 16": "hamming_dist_16",
    "Hamm. Dist. 25": "hamming_dist_25",
    "Hamm. Dist. 32": "hamming_dist_32",
    "L2 Distance 3": "l2_distance_3",
    "L2 Distance 4": "l2_distance_4",
    "L2 Distance 5": "l2_distance_5",
    "L2 Distance 8": "l2_distance_8",
    "L2 Distance 16": "l2_distance_16",
    "L2 Distance 25": "l2_distance_25",
    "L2 Distance 32": "l2_distance_32",
    "Linear Reg. 3": "lin_reg_3",
    "Linear Reg. 4": "lin_reg_4",
    "Linear Reg. 5": "lin_reg_5",
    "Linear Reg. 8": "lin_reg_8",
    "Linear Reg. 16": "lin_reg_16",
    "Linear Reg. 25": "lin_reg_25",
    "Linear Reg. 32": "lin_reg_32",
    "Poly. Reg. 3": "poly_reg_3",
    "Poly. Reg. 4": "poly_reg_4",
    "Poly. Reg. 5": "poly_reg_5",
    "Poly. Reg. 8": "poly_reg_8",
    "Poly. Reg. 16": "poly_reg_16",
    "Poly. Reg. 25": "poly_reg_25",
    "Poly. Reg. 32": "poly_reg_32",
    "Gx 3 Į- 3": "gx_kernel_3",
    "Gx 4 Į- 4": "gx_kernel_4",
    "Gx 5 Į- 5": "gx_kernel_5",
    "Gy 3 Į- 3": "gy_kernel_3",
    "Gy 4 Į- 4": "gy_kernel_4",
    "Gy 5 Į- 5": "gy_kernel_5",
    "Rob. Cross 3 Į- 3": "roberts_cross_3",
    "Rob. Cross 4 Į- 4": "roberts_cross_4",
    "Rob. Cross 5 Į- 5": "roberts_cross_5",
    "Mat. Mul. 3 Į- 3": "matrix_mul_3",
    "Mat. Mul. 4 Į- 4": "matrix_mul_4",
    "Mat. Mul. 5 Į- 5": "matrix_mul_5",
    "Sort 3": "sort_3",
    "Sort 4": "sort_4",
    "Max 5": "max_5",
    "Max 4": "max_4",
    "Max 3": "max_3",
    "Tree 50-50-5": "tree_50-50_5_1",
    "Tree 50-50-10": "tree_50-50_10_1",
    "Tree 100-50-5": "tree_100-50_5_1",
    "Tree 100-50-10": "tree_100-50_10_1",
    "Tree 100-100-5": "tree_100-100_5_1",
    "Tree 100-100-10": "tree_100-100_10_1",
}

COLUMN_BY_METRIC = {
    "compile": "compile_time (s)",
    "exec": "execution_time (s)",
    "noise": "Remaining_noise_budget",
}


def _parse_float(value: str) -> float:
    if value is None:
        return math.inf
    text = str(value).strip()
    if not text:
        return math.inf
    try:
        return float(text)
    except ValueError:
        if text.lower() == "inf":
            return math.inf
        return math.inf


def _load_rows(path: Path) -> Dict[str, Dict[str, str]]:
    rows: Dict[str, Dict[str, str]] = {}
    with path.open(newline="") as infile:
        reader = csv.DictReader(infile)
        for row in reader:
            benchmark = row.get("benchmark")
            if benchmark:
                rows[benchmark] = row
    return rows


def _extract_metric(
    rows: Dict[str, Dict[str, str]],
    column: str,
) -> Tuple[List[float], List[str]]:
    values: List[float] = []
    missing: List[str] = []
    for name in BENCHMARKS:
        key = CHEHAB_MAP.get(name)
        if key is None:
            values.append(math.inf)
            missing.append(name)
            continue
        row = rows.get(key)
        if row is None:
            values.append(math.inf)
            missing.append(name)
            continue
        values.append(_parse_float(row.get(column, "")))
    return values, missing


def _plot(
    label: str,
    values: List[float],
    metric: str,
    timeout_value: float,
    output: Path,
    show: bool,
) -> None:
    num_benchmarks = len(BENCHMARKS)
    x = np.arange(num_benchmarks)
    width = 0.6

    fig, ax = plt.subplots(figsize=(24, 3))
    hatch_color = (0.6, 0.6, 0.6, 1.0)
    hatch_linewidth = 4.0

    adjusted: List[float] = []
    hatches: List[str] = []
    for val in values:
        if math.isinf(val):
            adjusted.append(0.0 if metric == "noise" else timeout_value)
            hatches.append("//")
        else:
            adjusted.append(min(val, timeout_value))
            hatches.append("//" if val >= timeout_value else "")

    bars = ax.bar(x, adjusted, width, label=label, color="#1f77b4")
    for bar, hatch in zip(bars, hatches):
        if hatch:
            bar.set_hatch(hatch)
            bar._hatch_color = hatch_color
            bar._hatch_linewidth = hatch_linewidth

    ax.set_xticks(x)
    ax.set_xticklabels(BENCHMARKS, rotation=45, ha="right", fontsize=12)
    if metric == "exec":
        ylabel = "Execution Time (s)"
    elif metric == "compile":
        ylabel = "Compile Time (s)"
    else:
        ylabel = "Remaining Noise Budget"
    ax.set_ylabel(ylabel, fontsize=14)

    legend_label = "Timeout / missing"
    proxy = ax.bar(0, 0, color="none", hatch="//", edgecolor=hatch_color, label=legend_label)
    proxy._hatch_color = hatch_color
    proxy._hatch_linewidth = hatch_linewidth

    if metric in ("exec", "compile"):
        ax.set_yscale("log")
    ax.legend(fontsize=12, loc="upper center", bbox_to_anchor=(0.30, 1.05), ncol=2)
    plt.xlim(-0.5, len(x) - 0.5)
    if metric in ("exec", "compile"):
        plt.ylim(0, 2 * 10**3)
    else:
        finite = [v for v in adjusted if v > 0 and not math.isinf(v)]
        ymax = max(finite) if finite else 1.0
        plt.ylim(0, ymax * 1.15)
    plt.grid(axis="y", linestyle="--", linewidth=0.5, which="major")
    fig.tight_layout()

    plt.savefig(output, bbox_inches="tight")
    if show:
        plt.show()


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate CHEHAB graphs from CSV metrics.")
    parser.add_argument("--metric", choices=sorted(COLUMN_BY_METRIC.keys()), required=True)
    parser.add_argument("--csv", type=Path, required=True, help="Path to the CHEHAB CSV file.")
    parser.add_argument("--label", type=str, default="CHEHAB", help="Legend label for the series.")
    parser.add_argument("--timeout", type=float, default=7200.0, help="Cap for timeout/inf values.")
    parser.add_argument("--output", type=Path, default=Path("graph.png"), help="Output image path.")
    parser.add_argument("--show", action="store_true", help="Also display the plot interactively.")
    args = parser.parse_args()

    column = COLUMN_BY_METRIC[args.metric]
    rows = _load_rows(args.csv)
    values, missing = _extract_metric(rows, column)

    if missing:
        print(f"[warn] Missing entries for {args.label}: {', '.join(sorted(set(missing)))}")

    _plot(args.label, values, args.metric, args.timeout, args.output, args.show)
    print(f"Saved plot to {args.output}")


if __name__ == "__main__":
    main()
