"""
CoreMark comparison chart tool
Usage: python coremark_compare.py report1.txt report2.txt report3.txt
"""

import re
import sys
import matplotlib.pyplot as plt
import numpy as np

def parse_coremark(filepath):
    """Parse CoreMark report, extract iterations/sec and total time"""
    with open(filepath, 'r') as f:
        content = f.read()

    # Extract Iterations/Sec
    iter_match = re.search(r'Iterations/Sec\s+: ([\d\.]+)', content)
    if not iter_match:
        print(f"Warning: Cannot parse Iterations/Sec from {filepath}")
        return None, None

    iterations_sec = float(iter_match.group(1))

    # Extract Total time
    time_match = re.search(r'Total time \(secs\): ([\d\.]+)', content)
    if not time_match:
        print(f"Warning: Cannot parse Total time from {filepath}")
        return None, None

    total_time = float(time_match.group(1))

    return iterations_sec, total_time

def format_scientific(value):
    """Format number in scientific notation for large values"""
    if value >= 1e6 or value <= 1e-3:
        return f'{value:.2e}'
    else:
        return f'{value:.2f}'

def plot_compare(reports, labels=None, output="coremark_compare.png"):
    """Plot CoreMark comparison with grouped bars (dual axis)"""
    iterations = []
    times = []
    names = []

    for i, report in enumerate(reports):
        iter_sec, total_time = parse_coremark(report)
        if iter_sec is None or total_time is None:
            continue

        iterations.append(iter_sec)
        times.append(total_time)

        if labels and i < len(labels):
            names.append(labels[i])
        else:
            import os
            name = os.path.basename(report)
            name = os.path.splitext(name)[0]
            names.append(name)

    if len(iterations) == 0:
        print("Error: No valid CoreMark results found")
        sys.exit(1)

    # Create figure with dual axis
    fig, ax1 = plt.subplots(figsize=(14, 8))

    x = np.arange(len(names))
    width = 0.35

    # Bar chart - Iterations/Sec (left axis, left side of each group)
    bars1 = ax1.bar(x - width/2, iterations, width, label='Iterations/Sec',
                    color='#2E86AB', alpha=0.8, edgecolor='black', linewidth=0.5)
    ax1.set_ylabel('Iterations / Sec', fontsize=12, color='#2E86AB')
    ax1.tick_params(axis='y', labelcolor='#2E86AB')

    # Annotate Iterations values
    for bar, val in zip(bars1, iterations):
        ax1.text(bar.get_x() + bar.get_width()/2., val * 1.02,
                 format_scientific(val), ha='center', va='bottom', fontsize=9, color='#2E86AB')

    # Bar chart - Total Time (right axis, right side of each group)
    ax2 = ax1.twinx()
    bars2 = ax2.bar(x + width/2, times, width, label='Total Time (secs)',
                    color='#A23B72', alpha=0.8, edgecolor='black', linewidth=0.5)
    ax2.set_ylabel('Total Time (secs)', fontsize=12, color='#A23B72')
    ax2.tick_params(axis='y', labelcolor='#A23B72')

    # Annotate Time values
    for bar, val in zip(bars2, times):
        ax2.text(bar.get_x() + bar.get_width()/2., val * 1.02,
                 f'{val:.3f}', ha='center', va='bottom', fontsize=9, color='#A23B72')

    # X axis settings
    ax1.set_xticks(x)
    ax1.set_xticklabels(names, fontsize=11)
    ax1.set_xlabel('Benchmark Test', fontsize=12)
    ax1.set_title('CoreMark Performance Comparison', fontsize=14, fontweight='bold')
    ax1.grid(axis='x', alpha=0.3, linestyle='--')

    # Combine legends
    lines = [bars1, bars2]
    labels_legend = ['Iterations/Sec (higher is better)', 'Total Time (secs) (lower is better)']
    ax1.legend(lines, labels_legend, loc='upper left', fontsize=10)

    plt.tight_layout()
    plt.savefig(output, dpi=150, bbox_inches='tight')
    print(f"Saved: {output}")
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print("Usage: python coremark_compare.py report1.txt report2.txt report3.txt [label1] [label2] [label3]")
        print("Example: python coremark_compare.py gcc.txt clang.txt arm.txt GCC Clang ARM")
        sys.exit(1)

    reports = sys.argv[1:4]
    labels = sys.argv[4:7] if len(sys.argv) > 4 else None

    plot_compare(reports, labels)
