import argparse
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", type=str, required=True, help="Source file")
    args = parser.parse_args()

    input_file = args.file
    output_dir = "tmp"
    current_file = None
    content = []

    with open(input_file, "r", encoding="utf-8") as f:
        for line in f:
            line = line.rstrip("\n")
            if line.startswith("# @filename:"):
                # save previous file
                if current_file and content:
                    outpath = os.path.join(output_dir, current_file)
                    with open(outpath, "w", encoding="utf-8") as fw:
                        fw.write("\n".join(content) + "\n")
                    print(f"generate file: {outpath}")

                # reset
                filename = line.split()[-1].strip()
                current_file = filename
                content = []
            else:
                if current_file:
                    content.append(line)

    # save last one file
    if current_file and content:
        outpath = os.path.join(output_dir, current_file)
        with open(outpath, "w", encoding="utf-8") as fw:
            fw.write("\n".join(content) + "\n")
        print(f"generate file: {outpath}")
