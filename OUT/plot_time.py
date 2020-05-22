import csv
import matplotlib.pyplot as plt
import numpy as np

lab = ['VC_CNF_SAT', 'VC_APPROX1', 'VC_APPROX2']
mean = []
std = []
for k in range(5, 51, 5):
    data = [[], [], []] # sat_time, vc1_time, vc1_approx, vc2_time, vc2_approx
    with open('OUT_' + str(k)) as csvfile:
        readCSV = csv.reader(csvfile, delimiter=',')
        for row in readCSV:
            data[0].append(float(row[1]))
            data[1].append(float(row[2]))
            data[2].append(float(row[4]))

        arr = np.array(data)
        mean.append(np.mean(np.log10(arr), axis=1))
        std.append(np.std(np.log10(arr), axis=1))

m = np.array(mean)
s = np.array(std)
plt.figure(figsize=(12,7))
axes = plt.gca()
axes.set_xlim([0, 55])
axes.set_ylim([-6, 1])
plt.xticks(np.arange(0, 56, 5))
plt.yticks(np.arange(-6, 1.1))
for i in range(3): plt.errorbar(range(5, 51, 5), m[:, i], s[:, i], linestyle='-', marker='.', label=lab[i])
plt.xlabel("number of vertices")
plt.ylabel("log(time/s)")
plt.legend(loc = "upper left")
# plt.show()
plt.savefig('time_sc.png', bbox_inches='tight', dpi = 600)