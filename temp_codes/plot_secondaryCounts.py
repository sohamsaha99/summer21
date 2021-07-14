import pandas as pd
import plotly.graph_objects as go

def plot_secondaryCounts():
    infectiontypes = pd.read_csv("outputs/infectiontypes.txt", skipinitialspace=True)
    transmissions = pd.read_csv("outputs/transmissions.csv", skipinitialspace=True)
    tests = pd.read_csv("outputs/tests.txt", skipinitialspace=True)
    tests = tests.loc[tests['result']=="POSITIVE"]

    infectiontypes = infectiontypes.assign(
        secondary_count = infectiontypes['id'].apply(lambda y: (transmissions['source']==y).sum())
    ).assign(
        included = infectiontypes['id'].apply(lambda y: y in tests['id'])
    )
    df = pd.crosstab(infectiontypes.secondary_count, infectiontypes.included)
    df = df.assign(prop = df[True] / (df[True] + df[False])).reset_index()
    fig = go.Figure(data=go.Scatter(x=df.secondary_count, y=df['prop']))
    fig_df = go.Figure(data=[go.Table(
        header=dict(values=list(df.columns),
                    fill_color='paleturquoise',
                    align='left'),
        cells=dict(values=df.transpose().values.tolist(),
                fill_color='lavender',
                align='left'))
    ])
    return [fig, fig_df]


if __name__=='__main__':
    fig, fig_df = plot_secondaryCounts()
    fig.show()
    fig_df.show()
