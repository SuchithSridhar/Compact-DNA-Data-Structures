import csv
import random

# Define your parameters
k_values = [20]
insPar_values = [10]  # Possible values for insPar to vary
filter_values = [10000, 50000, 100000, 500000, 1000000]  # filter size
time_value = 1.0  # placeholder for time taken

# Create mem_size values that are >= k (incrementing by 5 or 10)
mem_size_values = [5, 10, 15, 20]  # Unique mem_size values

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
                        'time': time_value,
                    })

# Write to CSV file
csv_filename = 'data.csv'
with open(csv_filename, mode='w', newline='') as file:
    fieldnames = ['k', 'insPar', 'filter', 'mem_size', 'time']
    writer = csv.DictWriter(file, fieldnames=fieldnames)
    
    writer.writeheader()  # Write the header
    for row in data:
        writer.writerow(row)

print(f"CSV file '{csv_filename}' created successfully.")
