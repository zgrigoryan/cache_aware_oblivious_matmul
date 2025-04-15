import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('results.csv')

plt.plot(df['Size'], df['Naive'], label='Naive')
plt.plot(df['Size'], df['CacheAware'], label='Cache-Aware')
plt.plot(df['Size'], df['CacheOblivious'], label='Cache-Oblivious')
plt.plot(df['Size'], df['CacheAware1D'], label = 'Cache-Aware 1D')

plt.xlabel("Matrix Size (NxN)")
plt.ylabel("Time (ms)")
plt.title("Matrix Multiplication Timing Comparison")
plt.legend()
plt.grid(True)
plt.savefig("src/timing_plot.png") 
plt.show()
