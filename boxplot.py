import numpy as np

import pandas as pd

import matplotlib.pyplot as plt

# %matplotlib inline

import seaborn as sns

target_directory = '20180517V2'
output_directory = 'output' + '/' + target_directory + '/'
df = pd.read_csv(output_directory + 'questionnaire.csv')

print(df)

specific_case = pd.DataFrame([['1 camera', 'Q1', '4.23'],
                              ['1 camera', 'Q2', '1.69'],
                              ['No scheduling', 'Q1', '1.38'],
                              ['No scheduling', 'Q2', '3.08'],
                              ['minimum time 2s', 'Q1', '3.77'],
                              ['minimum time 2s', 'Q2', '4.15'],
                              ['minimum time 10s', 'Q1', '4.54'],
                              ['minimum time 10s', 'Q2', '4.31']
                              ],  columns = df.columns )

sns.stripplot(x='scheduling', hue='question', y='value', data=df, jitter=True,
              palette="Set2", split=True,linewidth=1,edgecolor='gray')
# sns.swarmplot(x='scheduling', hue='question', y='value', data=df)

ax = sns.boxplot(x='scheduling', hue='question', y='value', data=df,palette="Set2",fliersize=0)

plt.yticks(np.arange(1, 6, 1))
plt.xlabel("scheduling", fontsize=18)
plt.ylabel("value", fontsize=18)
plt.tick_params(labelsize=17)
plt.legend(bbox_to_anchor=(1, 0), loc='lower right', borderaxespad=1,fontsize=17)
plt.tight_layout()
plt.savefig(output_directory + 'boxplot.eps')
plt.show()