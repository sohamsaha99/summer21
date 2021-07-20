import os
import plotly.graph_objects as go
import csv

def plot_dailydata(output_dir):
    with open(os.path.join(output_dir, 'dailydata.txt'), newline='') as f:
        reader = csv.reader(f)
        data = list(reader)
        # print(reader)
    data = [[int(y) for y in x] for x in data[1:]]
    times=[]
    S=[]
    E=[]
    I=[]
    R=[]
    for d in data:
        times.append(d[0])
        S.append(d[1])
        E.append(d[2])
        I.append(d[3])
        R.append(d[4])
    N_people = S[0]+E[0]+I[0]+R[0]
    fig = go.Figure()
    fig_line = go.Figure()
    fig.add_trace(go.Scatter(
        x=times, y=I,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=0.5, color='rgb(241, 90, 90)'),
        name='I',
        stackgroup='one'
    ))
    fig.add_trace(go.Scatter(
        x=times, y=E,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=0.5, color='rgb(231, 231, 90)'),
        name='E',
        stackgroup='one'
    ))
    fig.add_trace(go.Scatter(
        x=times, y=R,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=0.5, color='rgb(90, 90, 90)'),
        name='R',
        stackgroup='one'
    ))
    fig.add_trace(go.Scatter(
        x=times, y=S,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=0.5, color='rgb(90, 238, 90)'),
        name='S',
        stackgroup='one'
    ))
    fig.update_layout(yaxis_range=(0, N_people), hovermode="x unified")
    fig_line.add_trace(go.Scatter(
        x=times, y=I,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=2, color='rgb(241, 90, 90)'),
        name='I'
    ))
    fig_line.add_trace(go.Scatter(
        x=times, y=E,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=2, color='rgb(90, 90, 231)'),
        name='E'
    ))
    fig_line.add_trace(go.Scatter(
        x=times, y=R,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=2, color='rgb(90, 90, 90)'),
        name='R'
    ))
    fig_line.add_trace(go.Scatter(
        x=times, y=S,
        hoverinfo='x+y',
        mode='lines',
        line=dict(width=2, color='rgb(90, 238, 90)'),
        name='S'
    ))
    fig_line.update_layout(yaxis_range=(0, N_people), hovermode="x unified")
    return [fig, fig_line]

if __name__=='__main__':
    fig, fig_line = plot_dailydata("../simulation_single_run/outputs/")
    fig.show()
    fig_line.show()

