import os
import pandas as pd
import plotly.graph_objects as go

def read_dataframes(output_dir):
    global contacts, infectiontypes, tests
    contacts = pd.read_csv(os.path.join(output_dir, "usefulcontacts.txt"), skipinitialspace=True)
    infectiontypes = pd.read_csv(os.path.join(output_dir, "infectiontypes.txt"), skipinitialspace=True)
    tests = pd.read_csv(os.path.join(output_dir, "tests.txt"), skipinitialspace=True)
    tests = tests.loc[tests['result']=="POSITIVE"]

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

def firstdegreecontacts(output_dir):
    read_dataframes(output_dir)
    df = infectiontypes.assign(
        first_degree = infectiontypes['id'].apply(lambda x: countFirstDegree(x))
    ).assign(
        inclusion = infectiontypes['id'].apply(lambda y: y in tests['id'].values)
    )
    df_tab = pd.crosstab(df.first_degree, df.inclusion)
    df_tab = df_tab.assign(prop = df_tab[True] / (df_tab[True] + df_tab[False])).reset_index()
    fig = go.Figure(data=go.Scatter(x=df_tab['first_degree'], y=df_tab['prop']))
    # fig.show()
    return fig

if __name__=='__main__':
    fig = firstdegreecontacts("../simulation_single_run/outputs/")
    fig.show()


