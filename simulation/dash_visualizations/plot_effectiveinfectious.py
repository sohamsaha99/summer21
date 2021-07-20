import os
import pandas as pd
import plotly.express as px

def read_dataframes(output_dir):
    global infectiontypes, transmissions, tests, quarantines
    infectiontypes = pd.read_csv(os.path.join(output_dir, "infectiontypes.txt"), skipinitialspace=True)
    transmissions = pd.read_csv(os.path.join(output_dir, "transmissions.csv"), skipinitialspace=True)
    tests = pd.read_csv(os.path.join(output_dir, "tests.txt"), skipinitialspace=True)
    tests = tests.loc[tests['result']=="POSITIVE"]
    quarantines = pd.read_csv(os.path.join(output_dir, "quarantines.txt"), skipinitialspace=True)

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
    for i in range(1, int(quarantines_subset.shape[0]/2) + 1):
        deducted_time += setIntersectionLength(begin, end, quarantines_subset.iloc[2*i-2]['time'], quarantines_subset.iloc[2*i-1]['time'])
    return (end - begin - deducted_time)

def countSecondaryInfections(i):
    return (transmissions['source']==i).sum()

def plot_effectiveinfectious(output_dir):
    read_dataframes(output_dir)
    infectiontypes_new = infectiontypes.assign(
        effective_infectious_time = infectiontypes.apply(lambda x: findUnquarantinePeriod(x['id'], x['presympt.time'], x['removed.time']), axis=1)
    ).assign(
        secondary_infections = infectiontypes.apply(lambda x: countSecondaryInfections(x['id']), axis=1)
    )
    infectiontypes_new['secondary_infections'] = infectiontypes_new['secondary_infections'].apply(lambda x: x if x < 10 else 10)
    infectiontypes_new = infectiontypes_new.assign(inclusion = infectiontypes_new['id'].apply(lambda y: y in tests['id'].values))
    fig1 = px.box(infectiontypes_new, x="inclusion", y="effective_infectious_time")
    fig2_1 = px.box(infectiontypes_new, x="secondary_infections", y="effective_infectious_time", title="Whole population")
    fig2_2 = px.box(infectiontypes_new.loc[infectiontypes_new['inclusion']], x="secondary_infections", y="effective_infectious_time", title="Included only")
    fig2_3 = px.box(infectiontypes_new.loc[~infectiontypes_new['inclusion']], x="secondary_infections", y="effective_infectious_time", title="Excluded only")
    return [fig1, fig2_1, fig2_2, fig2_3]

if __name__=='__main__':
    fig1, fig2_1, fig2_2, fig2_3 = plot_effectiveinfectious("../simulation_single_run/outputs/")
    # fig, fig_df = plot_secondaryCounts()
    fig1.show()
    fig2_1.show()
    fig2_2.show()
    fig2_3.show()
    # fig_df.show()
