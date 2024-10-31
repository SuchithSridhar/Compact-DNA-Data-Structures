import csv
import random

# Define your parameters
k_values = [5, 10, 15, 20]
insPar_values = [1, 2, 3, 4, 5, 6, 7, 8, 9 , 10] 
filter_values = [10000, 50000, 100000, 500000, 1000000] 
mem_size_values = [5, 10, 15, 20] 

# Create data for the CSV
data = []
for k in k_values:
    for mem in mem_size_values:
        if mem >= k:
            for insPar in insPar_values:
                for filter_val in filter_values:
                    data.append({
                        'k': k,
                        'insPar': insPar,
                        'filter': filter_val,
                        'mem_size': mem,
                    })

# Write to CSV file
csv_filename = 'data/input_data.csv'
with open(csv_filename, mode='w', newline='') as file:
    fieldnames = ['k', 'insPar', 'filter', 'mem_size']
    writer = csv.DictWriter(file, fieldnames=fieldnames)
    
    writer.writeheader()  # Write the header
    for row in data:
        writer.writerow(row)

print(f"CSV file '{csv_filename}' created successfully.")
