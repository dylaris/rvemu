import argparse

def parse_int(s):
    s = s.strip().lower()
    if s.startswith("0b"):
        return int(s, 2)
    elif s.startswith("0x"):
        return int(s, 16)
    else:
        return int(s)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", type=str, required=True, help="Source file path")
    parser.add_argument("-k", "--column", type=int, required=True, help="Which comma-separated column to sort (1-based)")
    parser.add_argument("-o", "--output", type=str, required=True, help="Output file path")
    parser.add_argument("-a", "--append", action="store_true", help="Append to output file instead of overwriting")

    args = parser.parse_args()

    with open(args.file, "r") as f:
        raw_lines = [l.rstrip("\n") for l in f.readlines() if l.strip()]

    new_lines = []
    for rl in raw_lines:
        parts = rl.split(",")
        col_idx = args.column - 1
        n = parts[col_idx].strip()
        opcode = parse_int(n)
        new_lines.append((n, rl))

    new_lines.sort()

    mode = "a" if args.append else "w"
    with open(args.output, mode, encoding="utf-8") as f:
        for op, line in new_lines:
            f.write(line + "\n")
