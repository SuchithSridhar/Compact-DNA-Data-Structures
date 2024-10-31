import subprocess
from tabulate import tabulate
import csv
from typing import List

_ = subprocess.run(["make", "clean"])
_ = subprocess.run(["make"])
_= subprocess.run(["mkdir", "tmp"])



# Define global variables
k: List[int] = []
insPar: List[int] = []
filter: List[int] = []
mem_size: List[int] = []
time: List[float] = []  # Define a global list for time

def read_csv(filename: str):
    global k, insPar, filter, mem_size, time  # Declare global variables to modify them

    with open(filename, mode='r') as file:
        csv_reader = csv.DictReader(file)
        for row in csv_reader:
            k.append(int(row['k']))
            insPar.append(int(row['insPar']))
            filter.append(1000000)
            # Handle empty values for mem_size while keeping the type as int
            mem_size.append(int(row['mem_size']) if row['mem_size'] else 0)  # Default to 0 for empty cells
            # Append the time, converting it to float
            time.append(float(row['time']) if row['time'] else 0.0)  # Default to 0.0 for empty cells



# Usage
read_csv('data.csv')

# Print global variables to verify
print("k:", k)
print("insPar:", insPar)
print("filter:", filter)
print("mem_size:", mem_size)
print("time:", time)  # Print the global time variable


patterns:list[str] = ["pattern.txt"]


def get_mems_info(output: str) -> tuple[str | None, str | None]:
    bw_steps = None
    time_taken = None

    for line in output.split("\n"):
        if "backward" in line:
            bw_steps = line.split(": ")[1]
        elif "Time" in line:
            time_taken = (line.split(": ")[1]).split(" m")[0]

    return (bw_steps, time_taken)


csv_filename = 'execution_data.csv'

with open(csv_filename, mode='w', newline='') as csvfile:
    # create a csv to store results
    fieldnames = ["k","insPar","filter","mem_size","time", "steps"]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()  # Write the header

    for i in range(len(k)):
        _ = subprocess.run(["./../build/build_kmer_filter", "../data/dataset.txt", 
                            f'tmp/dataset_{k[i]}_{insPar[i]}.blm', f'{k[i]}', f'{filter[i]}', f'{insPar[i]}'])

        with open(f'tmp/substring_{k[i]}_{insPar[i]}.txt', "w") as output_file: 
            _ = subprocess.run(["./../build/kmer_substrings", f'../data/{patterns[0]}', f'tmp/dataset_{k[i]}_{insPar[i]}.blm', f'{mem_size[i]}'], 
                            stdout=output_file, text=True)
        
        mems = subprocess.run(["./../build/move_mem_finder", "../data/dataset1.mvt",
                                "../data/tesatad1.mvt", f'tmp/substring_{k[i]}_{insPar[i]}.txt', f'{mem_size[i]}'], capture_output=True, text=True)
        
        bw_steps, time_taken = get_mems_info(mems.stdout)

        # Write to CSV for this k value
        writer.writerow({"k": k[i], "insPar": insPar[i], "filter": filter[i], "mem_size": mem_size[i], "time": time_taken, "steps": bw_steps})

        _ = subprocess.run(["rm", "-f", f'tmp/dataset_{k[i]}_{insPar[i]}.blm'])
        _ = subprocess.run(["rm" ,"-f", f'tmp/substring_{k[i]}_{insPar[i]}.txt'])

    print(f"CSV file '{csv_filename}' created successfully.")
