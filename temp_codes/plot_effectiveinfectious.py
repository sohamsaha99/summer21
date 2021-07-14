import pandas as pd
import plotly.graph_objects as go

infectiontypes = pd.read_csv("outputs/infectiontypes.txt", skipinitialspace=True)
transmissions = pd.read_csv("outputs/transmissions.csv", skipinitialspace=True)
tests = pd.read_csv("outputs/tests.txt", skipinitialspace=True)
tests = tests.loc[tests['result']=="POSITIVE"]
quarantines = pd.read_csv("outputs/quarantines.txt", skipinitialspace=True)

def setIntersectionLength(x1, y1, x2, y2):
    r = min(y1, y2) - max(x1, x2)
    if r >= 0:
        return r
    else:
        return 0

def findUnquarantinePeriod(i, begin, end):
    quarantines_subset = quarantines.loc[quarantines['id'] == i]
    if quarantines_subset.shape[0] == 0:
        return (end - begin)
    deducted_time = 0
    for i in range(1, quarantines_subset.shape[0]/2 + 1):
        deducted_time += setIntersectionLength(begin, end, quarantines_subset.iloc[2*i-2]['time'], quarantines_subset.iloc[2*i-1]['time'])
    return (end - begin - deducted_time)

def countSecondaryInfections(i):
    return (transmissions['source']==i).sum()

def plot_effectiveinfectious():
    infectiontypes_new = infectiontypes.assign(
        effective_infectious_time = infectiontypes.apply(lambda x: findUnquarantinePeriod(x['id'], x['presympt.time'], x['removed.time']), axis=1)
    ).assign(
        secondary_infections = infectiontypes.apply(lambda x: countSecondaryInfections(x['id']), axis=1)
    )
    infectiontypes_new['secondary_infections'] = infectiontypes_new['secondary_infections'].apply(lambda x: x if x < 10 else 10)
    infectiontypes_new = infectiontypes_new.assign(inclusion = infectiontypes_new['id'].apply(lambda y: y in tests['id'])

if __name__=='__main__':
    fig, fig_df = plot_secondaryCounts()
    fig.show()
    fig_df.show()
