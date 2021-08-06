# %%
from assign_direction import Node, criteria_function_1, criteria_function_2, find_source_of_node
import pandas as pd
import numpy as np
from typing import Set
import matplotlib.pyplot as plt
import math

# %%

# Common functions to use inside lambda
def SAR_function(set_of_contacts: Set, set_of_positive: Set):
    """
    ADD DOCSTRING
    """
    if len(set_of_contacts) == 0:
        return 0.0
    return len(set_of_positive)/len(set_of_contacts)

def make_node(arr: pd.Series):
    node = Node(given_id=arr["id"], raw_SAR=arr["secondary_attack_rate"], sample_collection_time=arr["collection.time"], index=arr.name)
    node.add_contacts(arr["set_of_positive"])
    return node

def check_estimated_source(arr: pd.Series, h_threshold=2):
    if (not math.isnan(arr["estimated_source"])) and np.abs(arr["source"] - arr["estimated_source"]) <= h_threshold:
        return True
    if (not arr["source_included"]) and math.isnan(arr["estimated_source"]):
        return True
    return False

# %%
def process_contacts(cases: pd.DataFrame, contacts_both_positive: pd.DataFrame):
    """
    ADD DOCSTRING
    """
    # Join to find case sample collection time
    contacts_both_positive = contacts_both_positive.merge(cases[["id", "collection.time"]], left_on="case.id", right_on="id", how="left").drop("id", axis=1)
    contacts_both_positive.rename(columns={"collection.time": "case_sample_time"}, inplace=True)

    # Join to find contact sample collection time
    contacts_both_positive = contacts_both_positive.merge(cases[["id", "collection.time"]], left_on="contact.id", right_on="id", how="left").drop("id", axis=1)
    contacts_both_positive.rename(columns={"collection.time": "contact_sample_time"}, inplace=True)

    # Find time difference of sample collection
    contacts_both_positive = contacts_both_positive.assign(sample_difference=(contacts_both_positive["case_sample_time"] - contacts_both_positive["contact_sample_time"]))
    return contacts_both_positive
    

# %%
def process_case_data(tests_file: str, contacts_file: str, transmissions_file: str):
    """
    ADD DOCSTRING
    """
    tests = pd.read_csv(tests_file, sep=",", skipinitialspace=True)
    # Subsetting tests data to find cases
    cases = tests[tests["result"] == "POSITIVE"]

    # Contact tracing data
    contacts = pd.read_csv(contacts_file, sep=",", skipinitialspace=True)

    # Info on source of each transmission
    transmissions = pd.read_csv(transmissions_file, sep=",", skipinitialspace=True)
    transmissions.rename(columns={"target": "id"}, inplace=True)
    transmissions = transmissions[["source", "id"]]

    # Subset of tracin data where contact id is posirtive
    contacts_both_positive = contacts[(contacts["contact.id"].isin(cases["id"]))]
    # Left join cases, transmission to include column of source for each case
    cases = cases.merge(transmissions, on="id", how="left")

    # Find sample collection time difference
    contacts_both_positive = process_contacts(cases, contacts_both_positive)
    # Subset if |difference|<=12
    contacts_subset = contacts_both_positive[np.abs(contacts_both_positive["sample_difference"]) <= 12]

    # Find set of contacts
    cases = cases.assign(set_of_contacts = cases["id"].apply(lambda x: set(contacts[contacts["case.id"] == x]["contact.id"])))
    cases = cases.assign(set2_of_contacts = cases["id"].apply(lambda x: set(contacts[contacts["contact.id"] == x]["case.id"])))
    cases = cases.assign(set_of_contacts = cases.apply(lambda x:x["set_of_contacts"].union(x["set2_of_contacts"]), axis=1)).drop(["set2_of_contacts"], axis=1)

    # Find set of positive contacts
    cases = cases.assign(set_of_positive = cases["id"].apply(lambda x: set(contacts_subset[contacts_subset["case.id"] == x]["contact.id"])))
    cases = cases.assign(set2_positive = cases["id"].apply(lambda x: set(contacts_subset[contacts_subset["contact.id"] == x]["case.id"])))
    cases = cases.assign(set_of_positive = cases.apply(lambda x: x["set_of_positive"].union(x["set2_positive"]), axis=1)).drop(["set2_positive"], axis=1)

    # Find raw secondary attack rate
    cases = cases.assign(secondary_attack_rate = cases.apply(lambda x: SAR_function(x["set_of_contacts"], x["set_of_positive"]), axis=1))

    return [cases, contacts_both_positive]



# %%
