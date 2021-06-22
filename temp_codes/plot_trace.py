import pandas as pd
import plotly.graph_objects as go

def plot_trace():
    try:
        df = pd.read_csv("trace.txt", skipinitialspace=True)
        tf = pd.read_csv("tests.txt", skipinitialspace=True)
        lf = pd.merge(df, tf, on="test.id")
        df_cross = pd.crosstab(lf['case.id'], lf['result'])
        degrees = list(df_cross["POSITIVE"])
        # degrees = [x for x in degrees if x>0]
        fig = go.Figure(data=[go.Histogram(x=degrees, histnorm='probability')])
        return fig
    except Exception as e:
        return {}

if __name__=='__main__':
    plot_trace().show()

