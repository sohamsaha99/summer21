import pandas as pd
import plotly.express as px

contacts = pd.read_csv("outputs/usefulcontacts.txt", skipinitialspace=True)
infectiontypes = pd.read_csv("outputs/infectiontypes.txt", skipinitialspace=True)

def checkRecallInterval(i, t):
    infectiontypes_subset = infectiontypes.loc[infectiontypes['id'] == i]
    if infectiontypes_subset.shape[0] > 0:
        presympt = float(infectiontypes_subset['presympt.time'])
        removed = float(infectiontypes_subset['removed.time'])
        if (t<removed) and (t>presympt-6):
            return True
        else:
            return False
    else:
        return False

def countFirstDegree(i):
    contacts_subset = contacts.loc[(contacts['P1'] == i) | (contacts['P2'] == i)]
    # print(contacts_subset)
    contacts_subset = contacts_subset.assign(neighbour = contacts_subset.apply(lambda x: x['P1'] if x['P2'] == i else x['P2'], axis=1))
    contacts_subset = contacts_subset.assign(useful = contacts_subset.apply(lambda x: checkRecallInterval(x['neighbour'], x['time']), axis=1))
    contacts_subset = contacts_subset.loc[contacts_subset['useful']]
    return len(set(list(contacts_subset['neighbour'])))

def firstdegreecontacts():
    infectiontypes_sub = infectiontypes.loc[infectiontypes['id']<100]
    df = infectiontypes_sub.assign(first_degree = infectiontypes_sub['id'].apply(lambda x: countFirstDegree(x)))
    return df