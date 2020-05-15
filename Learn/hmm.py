"""
Adapted from https://towardsdatascience.com/introduction-to-hidden-markov-models-cd2c93e6b781 May.2.20
"""

import numpy as np
import pandas as pd

obs_map = {'Cold': 0, 'Hot': 1}
obs = np.array([1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1])

inv_obs_map = dict((v, k) for k, v in obs_map.items())
obs_seq = [inv_obs_map[v] for v in list(obs)]

print("Simulated Observations:\n",pd.DataFrame(np.column_stack([obs, obs_seq]),columns=['Obs_code', 'Obs_seq']) )

pi = [0.6, 0.4]  # initial probabilities vector
states = ['Cold', 'Hot']
hidden_states = ['Snow', 'Rain', 'Sunshine']
pi = [0, 0.2, 0.8]
state_space = pd.Series(pi, index=hidden_states, name='states')
a_df = pd.DataFrame(columns=hidden_states, index=hidden_states)
a_df.loc[hidden_states[0]] = [0.3, 0.3, 0.4]
a_df.loc[hidden_states[1]] = [0.1, 0.45, 0.45]
a_df.loc[hidden_states[2]] = [0.2, 0.3, 0.5]
print("\n HMM matrix:\n", a_df)
a = a_df.values

observable_states = states
b_df = pd.DataFrame(columns=observable_states, index=hidden_states)
b_df.loc[hidden_states[0]] = [1,0]
b_df.loc[hidden_states[1]] = [0.8,0.2]
b_df.loc[hidden_states[2]] = [0.3,0.7]
print("\n Observable layer  matrix:\n",b_df)
b = b_df.values