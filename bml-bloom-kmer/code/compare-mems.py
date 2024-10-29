import sys
import re


def filter_acgt_strings(mems):
    mems_list = mems.split()
    filtered = []
    for mem in mems_list:
        pattern = r"^[ACGT]+"
        if re.match(pattern, mem):
            filtered.append(mem)

    return filtered


if len(sys.argv) != 3:
    print(f"Usage: {sys.argv[0]} <file 1> <file 2>")

file1 = sys.argv[1]
file2 = sys.argv[2]

with open(file1) as f:
    mem1 = f.read()

with open(file2) as f:
    mem2 = f.read()

mem1 = filter_acgt_strings(mem1)
mem2 = filter_acgt_strings(mem2)

same = 0
missing = 0

unpaired_mem1 = [x for x in mem1]
unpaired_mem2 = [x for x in mem2]

for mem in mem1:
    if mem in mem2:
        same += 1
        unpaired_mem1.remove(mem)
        unpaired_mem2.remove(mem)
    else:
        missing += 1


print(f"mems in 1 {sys.argv[1]}: {len(mem1)}")
print(f"mems in 2 {sys.argv[2]}: {len(mem2)}")
print(f"same mems: {same}")
print(f"missing mems: {missing}")
print("==== Unpaired mems1:")
print("\n".join(unpaired_mem1))
print("==== Unpaired mems1:")
print("\n".join(unpaired_mem2))
