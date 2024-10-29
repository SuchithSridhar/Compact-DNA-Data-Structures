import sys


if len(sys.argv) != 3:
    print(f"Usage: {sys.argv[0]} <input file> <output file>")

input_file = sys.argv[1]
output_file = sys.argv[2]

with open(input_file) as f:
    input = f.read()

input = input.split("\n")
input.sort(key=lambda x: len(x))

with open(output_file, "w") as f:
    f.write("\n".join(input))
