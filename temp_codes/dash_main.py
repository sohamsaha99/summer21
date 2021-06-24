from subprocess import Popen, PIPE, CalledProcessError
import plotly.graph_objects as go
import csv
import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State
from plot_dailydata import plot_dailydata
from plot_tests import plot_tests
from plot_transmission import plot_transmission
from plot_trace import plot_trace

external_stylesheets = ['https://codepen.io/chriddyp/pen/bWLwgP.css']

app = dash.Dash(__name__, external_stylesheets=external_stylesheets)

# app.layout = html.Div(children=[
#     html.H1(children='Hello Dash'),

#     html.Div(children='''
#         Dash: A web application framework for Python.
#     '''),

#     dcc.Graph(
#         id='example-graph',
#         figure={
#             'data': [
#                 {'x': [1, 2, 3], 'y': [4, 1, 2], 'type': 'bar', 'name': 'SF'},
#                 {'x': [1, 2, 3], 'y': [2, 4, 5], 'type': 'bar', 'name': u'Montréal'},
#             ],
#             'layout': {
#                 'title': 'Dash Data Visualization'
#             }
#         }
#     )
# ])

header_text = html.Div(children=
    [
        html.H1("Hello World, Hi World!"),
        html.Div("Hiiii!!"),
    ]
)
parameters = {
    "p_app_d": "0.0",
    "p_tested": "0.50",
    "p_test_high_contact": "0.95",
    "p_test_low_contact": "0.80",
    "p_traced": "0.0",
    "p_mask": "0.0",
    "test_delay_d": "1.0",
    "trace_delay_manual": "1.0",
    "trace_delay_app": "0.0",
    "manual_tracing_threshold": "0",
    "app_tracing_threshold": "0",
    "mask_reduction_out_d": "0.6",
    "mask_reduction_in_d": "0.9",
    "tracelength_d": "6",
    "quarantine_length": "14",
    "incubation_period": "5.2",
    "prodromal_period": "1.5",
    "p_asymptomatic": "0.60",
    "p_paucisymptomatic": "0.10",
    "p_mildsymptomatic": "0.29",
    "p_severesymptomatic": "0.01",
    "infectious_period": "4.3",
    "p_transmission": "0.3",
    "low_risk_adjustment": "0.7"
}
# parameter_keys = [key for key in parameters]
parameter_keys = ["p_app_d", "p_tested", "p_test_high_contact",
"p_test_low_contact", "p_traced", "p_mask", "test_delay_d",
"trace_delay_manual", "trace_delay_app", "manual_tracing_threshold",
"app_tracing_threshold", "mask_reduction_out_d", "mask_reduction_in_d",
"tracelength_d", "quarantine_length", "incubation_period", "prodromal_period",
"p_asymptomatic", "p_paucisymptomatic", "p_mildsymptomatic", "p_severesymptomatic",
"infectious_period", "p_transmission", "low_risk_adjustment"]
###########################################################################################################
###########################################################################################################
###########################################################################################################
div_elems = []
for key in parameter_keys:
    div_elems += [html.Label(key), dcc.Input(value=parameters[key], type="number", id=key, placeholder=key)]
input_fields = html.Div(children=div_elems, style={'columnCount': 4})
input_fieldss= html.Div(children=
    [
        html.Label("p_app_d"),
        dcc.Input(value="0.0", type="text", id="p_app_d"),
        html.Label("p_tested"),
        dcc.Input(value="0.50", type="text", id="p_tested"),
        html.Label("p_test_high_contact"),
        dcc.Input(value="0.95", type="text", id="p_test_high_contact"),
        html.Label("p_test_low_contact"),
        dcc.Input(value="0.80", type="text", id="p_test_low_contact"),
        html.Label("p_traced"),
        dcc.Input(value="0.0", type="text", id="p_traced"),
        html.Label("p_mask"),
        dcc.Input(value="0.0", type="text", id="p_mask"),
        html.Label("test_delay_d"),
        dcc.Input(value="1.0", type="text", id="test_delay_d"),
        html.Label("trace_delay_manual"),
        dcc.Input(value="1.0", type="text", id="trace_delay_manual"),
        html.Label("trace_delay_app"),
        dcc.Input(value="0.0", type="text", id="trace_delay_app"),
        html.Label("manual_tracing_threshold"),
        dcc.Input(value="0", type="text", id="manual_tracing_threshold"),
        html.Label("app_tracing_threshold"),
        dcc.Input(value="0", type="text", id="app_tracing_threshold"),
        html.Label("mask_reduction_out_d"),
        dcc.Input(value="0.6", type="text", id="mask_reduction_out_d"),
        html.Label("mask_reduction_in_d"),
        dcc.Input(value="0.9", type="text", id="mask_reduction_in_d"),
        html.Label("tracelength_d"),
        dcc.Input(value="6", type="text", id="tracelength_d"),
        html.Label("quarantine_length"),
        dcc.Input(value="14", type="text", id="quarantine_length"),
        html.Label("incubation_period"),
        dcc.Input(value="5.2", type="text", id="incubation_period"),
        html.Label("prodromal_period"),
        dcc.Input(value="1.5", type="text", id="prodromal_period"),
        html.Label("p_asymptomatic"),
        dcc.Input(value="0.60", type="text", id="p_asymptomatic"),
        html.Label("p_paucisymptomatic"),
        dcc.Input(value="0.10", type="text", id="p_paucisymptomatic"),
        html.Label("p_mildsymptomatic"),
        dcc.Input(value="0.29", type="text", id="p_mildsymptomatic"),
        html.Label("p_severesymptomatic"),
        dcc.Input(value="0.01", type="text", id="p_severesymptomatic"),
        html.Label("infectious_period"),
        dcc.Input(value="4.3", type="text", id="infectious_period"),
        html.Label("p_transmission"),
        dcc.Input(value="0.3", type="text", id="p_transmission"),
        html.Label("low_risk_adjustment"),
        dcc.Input(value="0.7", type="text", id="low_risk_adjustment"),
    ],
    style={'columnCount': 4}
)
button = html.Button(id='submit_button', n_clicks=0, children='Submit')
output_parameters = html.Div(id="output_parameters")
# running_status = html.Div(id="running_status")
dailydata_graph_stacked = dcc.Graph(id="dailydata_graph_stacked", style={'height': '100vh'})
dailydata_graph_line = dcc.Graph(id="dailydata_graph_line", style={'height': '100vh'})
tests_reason_graph = dcc.Graph(id="tests_reason_graph")
transmission_state_graph = dcc.Graph(id="transmission_state_graph")
test_trace_graph = html.Div(children=
    [
        html.Div(children=[tests_reason_graph], className="six columns"),
        html.Div(children=[transmission_state_graph], className="six columns")
    ],
    className="row"
)
trace_positive_graph = dcc.Graph(id="trace_positive_graph")
# download_tests_button = 
app.layout = html.Div(children=
    [
        header_text,
        input_fields,
        html.Br(),
        button,
        output_parameters,
        # running_status,
        dailydata_graph_stacked,
        dailydata_graph_line,
        test_trace_graph,
        trace_positive_graph
    ]
)
@app.callback(
    Output(component_id="output_parameters", component_property="children"),
    Input(component_id="submit_button", component_property="n_clicks"),
    [State(component_id=key, component_property="value") for key in parameter_keys]
)
def print_parameters(n_clicks, *vals):
    textfile = open("parameters_dash.txt", "w")
    for v in vals:
        textfile.write(str(v) + "\n")
    textfile.close()
    with Popen(["./a.out", "parameters_dash.txt"], stdout=PIPE, bufsize=1, universal_newlines=True) as p:
        for line in p.stdout:
            print(line, end='') # process line here

    if p.returncode != 0:
        raise CalledProcessError(p.returncode, p.args)
    return "After " + str(n_clicks) + " updates: " + " | ".join([str(v) for v in vals]) + ". Finished run."

@app.callback(
    Output(component_id="dailydata_graph_stacked", component_property="figure"),
    Output(component_id="dailydata_graph_line", component_property="figure"),
    Output(component_id="tests_reason_graph", component_property="figure"),
    Output(component_id="transmission_state_graph", component_property="figure"),
    Output(component_id="trace_positive_graph", component_property="figure"),
    Input(component_id="output_parameters", component_property="children")
)
def update_dailyplot(v):
    print("Updating plot after "+v)
    return plot_dailydata()+[plot_tests(), plot_transmission(), plot_trace()]

# @app.callback(
#     Output(component_id="tests_reason_graph", component_property="figure"),
#     Input(component_id="output_parameters", component_property="children")
# )
# def update_testsplot(v):
#     return plot_tests()


if __name__ == '__main__':
    # print(__name__)
    app.run_server(debug=True, port=8050)

