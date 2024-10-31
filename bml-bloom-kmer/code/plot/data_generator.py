import csv
import random

# Define your parameters
k_values = [1, 3, 5, 10, 13, 15, 20]
insPar_values = [1, 2, 3, 4, 5, 6, 7, 8, 9 , 10]  # Possible values for insPar to vary
filter_value = 1000000  # Constant filter size
time_value = 1.0  # Constant time

# Create mem_size values that are <= 40 and >= k (incrementing by 5 or 10)
# Start with 10 and go up to 40
mem_size_options = [5, 10, 15, 20] # More instances of 10
mem_size_values = sorted(set(mem_size_options))  # Unique mem_size values

# Create data for the CSV
data = []
for k in k_values:
    for mem in mem_size_values:
        if mem >= k:
            for insPar in insPar_values:
                data.append({
                    'k': k,
                    'insPar': insPar,
                    'filter': filter_value,
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
