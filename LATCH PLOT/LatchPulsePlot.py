import matplotlib.pyplot as plt
import csv
x = []
y = []
z = []
t = []
with open("C:/Users/mason/TestRig_MCU/LATCH PLOT/ADC_Capture.csv","r") as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        t.append(int(row[0]))
        x.append(int(row[1]))
        y.append(int(row[2]))
        z.append(int(row[3]))

PortA = plt.plot(t,x, label='PortA')
PortB = plt.plot(t,y, label='PortB')
Vf = plt.plot(t,z, label = 'V fuse')
plt.xlabel('Time (ms)')
plt.ylabel('ADC Value')
plt.title('ADC Capture')
plt.legend()
plt.show()