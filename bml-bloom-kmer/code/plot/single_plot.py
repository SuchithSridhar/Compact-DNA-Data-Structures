import pandas as pd
import matplotlib.pyplot as plt

# Load the data from the CSV file
csv_filename = 'data/output_data.csv'
data = pd.read_csv(csv_filename)

# Filter the data for the specific conditions
filtered_data = data[(data['mem_size'] == 20) & (data['insPar'] == 10) & (data['k'] == 20)]

# Create a figure
fig, ax = plt.subplots(figsize=(10, 6))

# Plot the data
ax.plot(filtered_data['filter'], filtered_data['steps'], marker='o', label='mem_size=20, insPar=10, k=20')

# Configure the plot
ax.set_xlabel('Filter Size')
ax.set_ylabel('Number of Backward Steps')
ax.set_title('Filter Size vs Number of Backward Steps')
ax.legend()
ax.grid()

# Save the plot to a file
output_filename = 'plots/filter_plot.png'  # Specify your desired file name and format
plt.savefig(output_filename, dpi=300, bbox_inches='tight')  # Save the figure

# Show the plot
# plt.show()
