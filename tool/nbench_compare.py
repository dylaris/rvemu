"""
nbench comparison chart tool
Usage: python nbench_compare.py report1.txt report2.txt
"""

import re
import sys
import matplotlib.pyplot as plt
import numpy as np

def parse_nbench(filepath):
    """Parse nbench report, return test names, iterations and New Index"""
    with open(filepath, 'r') as f:
        content = f.read()

    tests = []
    iterations = []
    new_indices = []

    # Match: TEST NAME : iterations/sec : Old Index : New Index
    pattern = r'([A-Z][A-Z\s]+?)\s+:\s+([\d\.eE+-]+)\s+:\s+[\d\.e+-]+\s+:\s+([\d\.eE+-]+)'

    for match in re.finditer(pattern, content):
        tests.append(match.group(1).strip())
        iterations.append(float(match.group(2)))
        new_indices.append(float(match.group(3)))

    return tests, iterations, new_indices

def format_scientific(value):
    """Format number in scientific notation"""
    if value >= 1e6 or value <= 1e-3:
        return f'{value:.2e}'
    else:
        return f'{value:.2f}'

def plot_compare(report1, report2, label1="Device A", label2="Device B", output="nbench_compare.png"):
    """Plot comparison chart (bar + line, dual axis)"""
    tests1, iter1, index1 = parse_nbench(report1)
    tests2, iter2, index2 = parse_nbench(report2)

    # Ensure test items match
    if tests1 != tests2:
        print("Warning: Test items not identical, using first report as reference")
        idx_map = {name: i for i, name in enumerate(tests2)}
        tests = tests1
        iter2 = [iter2[idx_map.get(t, 0)] for t in tests]
        index2 = [index2[idx_map.get(t, 0)] for t in tests]
    else:
        tests = tests1

    x = np.arange(len(tests))
    width = 0.35

    fig, ax1 = plt.subplots(figsize=(16, 9))

    # Bar chart - iterations/sec (left axis)
    bars1 = ax1.bar(x - width/2, iter1, width, label=f'{label1} (iter/sec)',
                    color='#2E86AB', alpha=0.7, edgecolor='black', linewidth=0.5)
    bars2 = ax1.bar(x + width/2, iter2, width, label=f'{label2} (iter/sec)',
                    color='#A23B72', alpha=0.7, edgecolor='black', linewidth=0.5)
    ax1.set_ylabel('Iterations / sec', fontsize=12, color='black')
    ax1.tick_params(axis='y', labelcolor='black')

    # Use log scale for iterations to show all bars
    ax1.set_yscale('log')

    # Annotate bar values, no rotation
    for bar in bars1:
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height * 1.1,
                 format_scientific(height), ha='center', va='bottom', fontsize=8)
    for bar in bars2:
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height * 1.1,
                 format_scientific(height), ha='center', va='bottom', fontsize=8)

    # Line chart - New Index (right axis)
    ax2 = ax1.twinx()
    line1, = ax2.plot(x, index1, 'o-', label=f'{label1} (New Index)',
                      color='#1B4965', linewidth=2, markersize=8)
    line2, = ax2.plot(x, index2, 's-', label=f'{label2} (New Index)',
                      color='#5F0F40', linewidth=2, markersize=8)
    ax2.set_ylabel('New Index', fontsize=12, color='#1B4965')
    ax2.tick_params(axis='y', labelcolor='#1B4965')

    # Annotate line values
    for i, (ix, val) in enumerate(zip(x, index1)):
        ax2.annotate(f'{val:.2f}', (ix, val), textcoords="offset points",
                     xytext=(0, 10), ha='center', fontsize=8, color='#1B4965')
    for i, (ix, val) in enumerate(zip(x, index2)):
        ax2.annotate(f'{val:.2f}', (ix, val), textcoords="offset points",
                     xytext=(0, -15), ha='center', fontsize=8, color='#5F0F40')

    # X axis settings
    ax1.set_xticks(x)
    ax1.set_xticklabels(tests, rotation=45, ha='right', fontsize=9)
    ax1.set_xlabel('Benchmark Test', fontsize=12)

    # Combine legends
    lines = [bars1, bars2, line1, line2]
    labels = [f'{label1} (iter/sec)', f'{label2} (iter/sec)',
              f'{label1} (New Index)', f'{label2} (New Index)']
    ax1.legend(lines, labels, loc='upper left', fontsize=9)

    ax1.set_title('nbench Performance Comparison\n(Bar: Throughput, Line: Normalized Performance)',
                  fontsize=14, fontweight='bold')
    ax1.grid(axis='x', alpha=0.3, linestyle='--')

    plt.tight_layout()
    plt.savefig(output, dpi=150, bbox_inches='tight')
    print(f"Saved: {output}")
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python nbench_compare.py report1.txt report2.txt [label1] [label2]")
        print("Example: python nbench_compare.py pentium.txt k6.txt Pentium K6")
        sys.exit(1)

    label1 = sys.argv[3] if len(sys.argv) > 3 else "Device A"
    label2 = sys.argv[4] if len(sys.argv) > 4 else "Device B"

    plot_compare(sys.argv[1], sys.argv[2], label1, label2)
