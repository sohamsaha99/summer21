# %%
from typing import Dict

from plotnine.mapping.evaluation import after_stat
from assign_direction import *
from process_data import *
from plotnine import ggplot, geom_point, aes, stat_smooth, facet_wrap, geom_histogram

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
df = df.merge(infectiontypes[["id", "symptom.type", "sympt.time"]], how="left", on="id")
# %%
pd.crosstab(df["check_result"], df["symptom.type"])
# %%
pd.crosstab(df["check_result"], df["source_included"])

# %%
def find_array_of_attribute(set_of_positive: Set, source: int, dictionary_of_attribute: Dict):
    """
    ADD DOCSTRING
    """
    array_of_attribute = []
    for idx in set_of_positive:
        if idx != source:
            array_of_attribute.append(dictionary_of_attribute[idx])
    return array_of_attribute

def get_difference_of_attribute(df: pd.DataFrame, attribute: str, binwidth=None):
    """
    ADD DOCSTRING
    """
    dictionary_of_attribute = df.set_index("id", drop=True).apply(lambda x: x[attribute], axis=1).to_dict()
    df["array_of_"+attribute] = df.apply(lambda x: find_array_of_attribute(x["set_of_positive"], x["source"], dictionary_of_attribute), axis=1)
    df["source_"+attribute] = df["source"].apply(lambda x: dictionary_of_attribute.get(x))
    df[attribute+"_difference"] = df.apply(lambda x: [i - x["source_"+attribute] for i in x["array_of_"+attribute]], axis=1)
    array_of_attribute_differences = []
    for _, row in df[df["source_included"]].iterrows():
        array_of_attribute_differences += row[attribute+"_difference"]
    # plt.hist(array_of_attribute_differences)
    dat = pd.DataFrame({'Difference of '+attribute: array_of_attribute_differences})
    (ggplot(dat, aes(x='Difference of '+attribute, y=after_stat("density")))
    + geom_histogram(binwidth=binwidth)
    ).draw()
    print(pd.Series(array_of_attribute_differences).describe())

# %%
get_difference_of_attribute(df, "sympt.time")
# %%
get_difference_of_attribute(df, "secondary_attack_rate", 0.2)
# %%
get_difference_of_attribute(df, "collection.time", 2.0)
# %%
