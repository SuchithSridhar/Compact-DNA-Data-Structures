import pandas as pd
import matplotlib.pyplot as plt
import itertools

# Load the data from the CSV file
csv_filename = 'data/output_data.csv'
data = pd.read_csv(csv_filename)

# Get unique values for mem_size, k, and insPar
mem_values = data['mem_size'].unique()
k_values = data['k'].unique()
insPar_values = data['insPar'].unique()

# Create a figure
fig, axs = plt.subplots(len(mem_values), len(k_values), figsize=(15, 10), constrained_layout=True)
fig.suptitle('Number of Backward Steps vs Filter Size for Different values of MEM size, K-mer size, and Insertion Parameter')

# Iterate through all combinations of mem_size, k, and insPar
for (i, mem_value), (j, k_value) in itertools.product(enumerate(mem_values), enumerate(k_values)):
    for insPar_value in insPar_values:
        # Filter the data for the specific conditions
        filtered_data = data[(data['mem_size'] == mem_value) & (data['insPar'] == insPar_value) & (data['k'] == k_value)]
        
        # Plot the data
        ax = axs[i, j] if len(mem_values) > 1 and len(k_values) > 1 else axs[max(i, j)]
        ax.plot(filtered_data['filter'], filtered_data['steps'], marker='o', label='insPar={0}'.format(insPar_value))
        
        # Configure the plot
        ax.set_xlabel('Filter Size')
        ax.set_ylabel('Number of Backward Steps')
        ax.set_title('mem size={0}, k={1}'.format(mem_value, k_value))
        ax.legend(prop={'size': 6})  # Adjust the size of the legend
        ax.grid()

# Save the plot to a file
output_filename = 'plots/filter_plot_combined.png'  # Specify your desired file name and format
plt.savefig(output_filename, dpi=300, bbox_inches='tight')  # Save the figure

# Show the plot
# plt.show()
