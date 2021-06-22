import json
import plotly.graph_objects as go

def plot_transmission():
    with open("graphdata.json") as f:
        data=json.load(f)
    I = 0
    J = 0
    K = 0
    L = 0
    M = 0
    for l in data['links']:
        if l['state'] == 'I':
            I += 1
        elif l['state'] == 'J':
            J += 1
        elif l['state'] == 'K':
            K += 1
        elif l['state'] == 'L':
            L += 1
        elif l['state'] == 'M':
            M += 1
    fig = go.Figure([go.Bar(x=['presympt.', 'asympt.', 'Sympt.'], y=[I, J, 0], name=""),
                    go.Bar(x=['presympt.', 'asympt.', 'Sympt.'], y=[0, 0, K], name="Pauci"),
                    go.Bar(x=['presympt.', 'asympt.', 'Sympt.'], y=[0, 0, L], name="Mild"),
                    go.Bar(x=['presympt.', 'asympt.', 'Sympt.'], y=[0, 0, M], name="Severe")
        ])
    fig.update_layout(barmode="stack" ,showlegend=False)
    return fig

if __name__=='__main__':
    plot_transmission().show()

