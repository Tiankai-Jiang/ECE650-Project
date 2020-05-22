import csv
import matplotlib.pyplot as plt
import numpy as np

data = [[], [], [], [], []]
with open('OUT_large') as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')
    for row in readCSV:
        for i in range(5):
            data[i].append(float(row[i]))

plt.figure(figsize=(12,7))
axes = plt.gca()
axes.set_xlim([0, 2001])
axes.set_ylim([0, 0.5])
plt.xticks(np.arange(0, 2001, 100))
plt.yticks(np.arange(0, 0.6, 0.1))
plt.plot(range(50, 2001, 25), data[1], linestyle='-', marker='', label="Greedy")
plt.plot(range(50, 2001, 25), data[3], linestyle='-', marker='', label="Random")
plt.xlabel("number of vertices")
plt.ylabel("time/s")
plt.legend(loc = "upper left")
plt.savefig('large_time.png', bbox_inches='tight', dpi = 600)
plt.clf()

axes = plt.gca()
axes.set_xlim([0, 2001])
axes.set_ylim([0, 1600])
plt.xticks(np.arange(0, 2001, 100))
plt.yticks(np.arange(0, 1601, 100))
plt.plot(range(50, 2001, 25), data[2], linestyle='-', marker='', label="Greedy")
plt.plot(range(50, 2001, 25), data[4], linestyle='-', marker='', label="Random")
plt.xlabel("number of vertices")
plt.ylabel("vertices selected")
plt.legend(loc = "upper left")
plt.savefig('large_ratio.png', bbox_inches='tight', dpi = 600)