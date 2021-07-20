import os
import pandas as pd
import plotly.graph_objects as go

def plot_tests(output_dir):
    df = pd.read_csv(os.path.join(output_dir, "tests.txt"), skipinitialspace=True)
    df_cross = pd.crosstab(df.reason, df.result)
    # initiate data list for figure
    data = []
    #use for loop on every zoo name to create bar data
    for x in df_cross.columns:
       data.append(go.Bar(name=str(x), x=df_cross.index, y=df_cross[x]))

    figure = go.Figure(data)
    figure.update_layout(barmode = 'stack')

    #For you to take a look at the result use
    return figure

if __name__=='__main__':
    plot_tests("../simulation_single_run/outputs/").show()

