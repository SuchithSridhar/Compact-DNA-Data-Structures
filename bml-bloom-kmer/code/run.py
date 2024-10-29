import subprocess
from tabulate import tabulate

_ = subprocess.run(["make", "clean"])
_ = subprocess.run(["make"])
_= subprocess.run(["mkdir", "tmp"])

print("\n\n")
k_input :str = input("Enter a list of Kmer sizes to test, seperated by spaces\n")
k :list[int] = list(map(int, k_input.split()))

print("\n")
insPar_input :str = input("Enter a list of insParams to test, seperated by spaces\n")
insPar :list[int] = list(map(int, insPar_input.split()))

table_head = ["#", "mems using kmers", "mems using MVT", "time using kmers", "time using MVT"]

_ = subprocess.run(["./build/build_kmer_filer"])
