## Testing the outputs from C++

# %%
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# %%
df = pd.read_csv("projects/summer21/simulation_testing/outputs/result.txt", header=None)

# %%
print(np.mean(df[0]))
print(np.std(df[0]))
# %%
df[0].hist()

# %%
plt.hist(np.log10(df[0]))
# %%
