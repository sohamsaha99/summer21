import pandas as pd
import numpy as np

def process_case_data(tests_file: str, contacts_file: str, transmissions_file: str):
    tests = pd.read_csv(tests_file, sep=",", skipinitialspace=True)
    cases = tests[tests["result"] == "POSITIVE"]
    contacts = pd.read_csv(contacts_file, sep=",", skipinitialspace=True)
    transmissions = pd.read_csv(transmissions_file, sep=",", skipinitialspace=True)
    transmissions.rename({"target": "id"}, inplace=True)
    transmissions = transmissions[["source", "id"]]
    cases = cases.join(transmissions, on="id", how="left")

