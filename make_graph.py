import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd

target_directory = '20181023'
mode = 'mintime'
output_directory = 'output' + '/' + target_directory + '/'

plt.figure(figsize=(35,8),dpi=100)
# plt.figure(figsize=(35,20),dpi=100)
plt.style.use('default')
sns.set()
sns.set_style('whitegrid')
sns.set_palette('Set1')

df_switch_camera = pd.read_csv(output_directory + mode + '_optimize.csv')
df_head = pd.read_csv (output_directory + 'head.csv', header=None)


start = 2600
end = 15100
fps = 25
# start = 0
# end = np.array(df_switch_camera['topranks']).size

x = np.linspace(0, (end - start) / fps, end - start)
colorlist = ["r", "g", "b"]
# print(df_switch_camera.columns)
list_head = []
for i in df_head.columns:
    # plt.plot(x, np.array(df_head[i])[start:end], label = 'camera ' + str(i))
    list_head.append(np.array(df_head[i])[start:end])

array_head = np.array(list_head)

array20 = np.array(df_switch_camera['20'])[start:end]
array10 = np.array(df_switch_camera['10'])[start:end]
array5 = np.array(df_switch_camera['5'])[start:end]
array2 = np.array(df_switch_camera['2'])[start:end]
arraytop = np.array(df_switch_camera['topranks'])[start:end]

list10 = []
list2 = []
list20 = []
list5 = []
listtop = []

for i in range(array10.size):
    list10.append(array_head[array10[i], i])
    list2.append(array_head[array2[i], i])
    listtop.append(array_head[arraytop[i], i])
    list20.append(array_head[array20[i], i])
    list5.append(array_head[array5[i], i])


plt.plot(x, np.array(list10), label = 'minimum time 10s', lw = 5)
# plt.plot(x, np.array(list2), label = 'minimum time 2s', lw = 2)
plt.plot(x, np.array(listtop), label = 'No scheduling', lw = 2)

# plt.plot(x, np.array(list20), label = 'minimum time 20s', lw = 7)
# plt.plot(x, np.array(list10), label = 'minimum time 10s', lw = 5)
# plt.plot(x, np.array(list5), label = 'minimum time 5s', lw = 3)
# plt.plot(x, np.array(listtop), label = 'No scheduling', lw = 1)

# print(np.sum(np.array(list20))/array20.size)
# print(np.sum(np.array(list10))/array20.size)
# print(np.sum(np.array(list5))/array20.size)
# print(np.sum(np.array(listtop))/array20.size)

# plt.plot(x, np.array(df_switch_camera['20'])[start:end] , label = 'minimum time 20s', lw = 10)
# plt.plot(x, np.array(df_switch_camera['10'])[start:end] , label = 'minimum time 10s', lw = 7)
# plt.plot(x, np.array(df_switch_camera['5'])[start:end], label = 'minimum time 5s', lw = 4)
# plt.plot(x, np.array(df_switch_camera['2'])[start:end], label = 'minimum time 2s', lw = 6)
# plt.plot(x, np.array(df_switch_camera['topranks'])[start:end], label = 'No scheduling')
# plt.plot(x, np.array(df_switch_camera['2'])[start:end], label = 'minimum time 2s', lw = 6)


plt.legend()
plt.xlabel("time [s]",fontsize=36)
# plt.ylabel("camera number",fontsize=36)
plt.ylabel("negative region",fontsize=36)
# plt.ylim(-0.2, 5.2)
plt.ylim(0.25, 1.05)
plt.legend(fontsize=36)
plt.tick_params(labelsize=36)
# plt.legend(bbox_to_anchor=(0, 1), loc='upper left', borderaxespad=1,fontsize=36)
plt.tight_layout()
# plt.savefig(output_directory + mode + 'x_0-500.png')
plt.savefig(output_directory + 'negativea_0-500.png')
plt.show()
