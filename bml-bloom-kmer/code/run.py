import subprocess
from tabulate import tabulate

_ = subprocess.run(["make", "clean"])
_ = subprocess.run(["make"])
_ = subprocess.run(["mkdir", "tmp"])

k: list[int] = [10, 20, 8]
insPar: list[int] = [4, 4, 4]
mem_size = 20

patterns: list[str] = ["single_pattern.txt", "pattern.txt"]

table: list[list[str]] = []
table_head = ["pattern", "MEM Size", "Steps without Kmers"]


def get_mems_info(output: str) -> tuple[str | None, str | None]:
    bw_steps = None
    time_taken = None

    for line in output.split("\n"):
        if "backward" in line:
            bw_steps = line.split(": ")[1]
        elif "Time" in line:
            time_taken = line.split(": ")[1]

    return (bw_steps, time_taken)


for i in range(0, len(k)):
    table_head.append(f"kmer size={k[i]}, Insertion Param={insPar[i]}")

print("\n")

# Building the filters for the (k, insPar, memSize) tuples, then building the substring
for pattern_num in range(0, len(patterns)):
    table.append([])
    table[pattern_num].append(patterns[pattern_num])
    table[pattern_num].append(str(mem_size))

    print(f"Finding the reference mems for the data/{patterns[pattern_num]}")

    mems = subprocess.run(
        [
            "./build/move_mem_finder",
            "data/dataset1.mvt",
            "data/tesatad1.mvt",
            f"data/{patterns[pattern_num]}",
            f"{mem_size}",
        ],
        capture_output=True,
        text=True,
    )

    bw_steps, time_taken = get_mems_info(mems.stdout)
    table_input = f"{bw_steps} steps, {time_taken}"
    table[pattern_num].append(table_input)

    for i in range(0, len(k)):
        _ = subprocess.run(
            [
                "./build/build_kmer_filter",
                "data/dataset.txt",
                f"tmp/dataset_{k[i]}_{insPar[i]}.blm",
                f"{k[i]}",
                "500000",
                f"{insPar[i]}",
            ]
        )

        with open(f"tmp/substring_{k[i]}_{insPar[i]}.txt", "w") as output_file:
            _ = subprocess.run(
                [
                    "./build/kmer_substrings",
                    f"data/{patterns[pattern_num]}",
                    f"tmp/dataset_{k[i]}_{insPar[i]}.blm",
                    f"{mem_size}",
                ],
                stdout=output_file,
                text=True,
            )

        mems = subprocess.run(
            [
                "./build/move_mem_finder",
                "data/dataset1.mvt",
                "data/tesatad1.mvt",
                f"tmp/substring_{k[i]}_{insPar[i]}.txt",
                f"{mem_size}",
            ],
            capture_output=True,
            text=True,
        )

        bw_steps, time_taken = get_mems_info(mems.stdout)
        table_input = f"{bw_steps} steps, {time_taken}"
        table[pattern_num].append(table_input)


print(tabulate(table, headers=table_head))
