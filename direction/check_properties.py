# %%
from typing import Dict
from assign_direction import *
from process_data import *

# %%
cases, contacts = process_case_data("datasets/tests.txt", "datasets/trace.txt", "datasets/transmissions.csv")

# %%
# Create vector of nodes
dictionary_of_nodes = cases.set_index("id", drop=False).apply(lambda x: make_node(x), axis=1).to_dict()

# %%
# Find source of each node using dictionary_of_nodes
for idx in cases["id"]:
    # print("Doing {}".format(idx))
    find_source_of_node(idx, dictionary_of_nodes, criteria_function_1)
# %%
cases = cases.assign(estimated_source = cases["id"].apply(lambda x: dictionary_of_nodes[x].source))
# %%
print((np.abs(cases["source"] - cases["estimated_source"]) <= 2).sum())
print((cases["source"] == cases["estimated_source"]).sum())
print(cases["estimated_source"].isna().sum())
(cases["source"] - cases["estimated_source"]).hist()
plt.title('Difference between true source and estimated source')
plt.xlabel('true source - estimated source')
# %%
cases = cases.assign(source_included = cases["source"].isin(cases["id"]))

# %%
print(cases.apply(lambda x: check_estimated_source(x), axis=1).sum())

# %%
df = cases.assign(check_result = cases.apply(lambda x: check_estimated_source(x, 3), axis=1))
infectiontypes = pd.read_csv("datasets/infectiontypes.txt", skipinitialspace=True, sep=",")
df = df.merge(infectiontypes[["id", "symptom.type"]], how="left", on="id")
# %%
pd.crosstab(df["check_result"], df["symptom.type"])
# %%
pd.crosstab(df["check_result"], df["source_included"])

# %%
# Creates a dictionary with key: id, value: SAR
dictionary_of_SAR = df.set_index("id", drop=True).apply(lambda x: x["secondary_attack_rate"], axis=1).to_dict()

# %%
def find_array_of_SAR(set_of_positive: Set, source: int, dictionary_of_SAR: Dict):
    """
    ADD DOCSTRING
    """
    array_of_SAR = []
    for idx in set_of_positive:
        if idx != source:
            array_of_SAR.append(dictionary_of_SAR[idx])
    return array_of_SAR

df["array_of_SAR"] = df.apply(lambda x: find_array_of_SAR(x["set_of_positive"], x["source"], dictionary_of_SAR), axis=1)
df["source_SAR"] = df["source"].apply(lambda x: dictionary_of_SAR.get(x))
df["SAR_difference"] = df.apply(lambda x: [i - x["source_SAR"] for i in x["array_of_SAR"]], axis=1)
# %%
array_of_SAR_differences = []
for _, row in df[df["source_included"]].iterrows():
    array_of_SAR_differences += row["SAR_difference"]

# %%
plt.hist(array_of_SAR_differences)
pd.Series(array_of_SAR_differences).describe()

# %%
