import pandas as pd
import matplotlib.pyplot as plt

# Load the data from the CSV file
csv_filename = 'data/output_data.csv'
data = pd.read_csv(csv_filename)

# Get unique mem_size values
mem_sizes = data['mem_size'].unique()

# Calculate the number of rows needed for subplots
n_rows = len(mem_sizes)

# Create a figure with smaller subplots stacked vertically
fig, axs = plt.subplots(n_rows, 2, figsize=(10, n_rows * 4))  # Reduced figsize

# Define axis limits for consistency
xlim_k = (min(data['k']), max(data['k']))  # X-axis limits for Kmer Size
xlim_insPar = (min(data['insPar']), max(data['insPar']))  # X-axis limits for Insertion Parameter
ylim = (0, data['steps'].max() + 10)  # Y-axis limits for Number of Steps

# filter by filter
chosen_filter = 1000000

# Plot for each mem_size
for i, mem_size in enumerate(mem_sizes):
    # Filter the data for the current mem_size
    filtered_data = data[data['mem_size'] == mem_size]
    filtered_data = filtered_data[filtered_data['filter'] == chosen_filter]
    
    # Plot for Number of Steps vs Kmer Size
    for insPar_value, group in filtered_data.groupby('insPar'):
        axs[i, 0].plot(group['k'], group['steps'], marker='o', label=f'insPar = {insPar_value}')

    # Configure first plot
    axs[i, 0].set_xlabel('Kmer Size')
    axs[i, 0].set_ylabel('Number of Backward Steps')
    axs[i, 0].set_title(f'mem size = {mem_size}\nfilter size = {chosen_filter/8/1000} KB\n\nKmer Size vs Backward Steps')
    axs[i, 0].set_xlim(xlim_k)  # Set x-axis limits
    axs[i, 0].set_ylim(ylim)  # Set y-axis limits
    axs[i, 0].legend()
    axs[i, 0].grid()

    # Plot for Number of Steps vs Insertion Parameters
    for k_value, group in filtered_data.groupby('k'):
        axs[i, 1].plot(group['insPar'], group['steps'], marker='o', label=f'k = {k_value}')

    # Configure second plot
    axs[i, 1].set_xlabel('Insertion Parameter')
    axs[i, 1].set_ylabel('Number of Backward Steps')
    axs[i, 1].set_title(f'mem size = {mem_size}\nfilter size = {chosen_filter/8/1000} KB\n\nInstance Parameter vs Backward Steps')
    axs[i, 1].set_xlim(xlim_insPar)  # Set x-axis limits
    axs[i, 1].set_ylim(ylim)  # Set y-axis limits
    axs[i, 1].legend()
    axs[i, 1].grid()

# Adjust layout to prevent overlap
plt.tight_layout(pad=2.0)  # Increase padding between plots

# Save the combined plots to a file
output_filename = 'plots/combined_plots.png'  # Specify your desired file name and format
plt.savefig(output_filename, dpi=300, bbox_inches='tight')  # Save the figure

# Show the combined plots
# plt.show()
