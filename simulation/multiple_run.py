import os
import shutil
from subprocess import call

day = 24*60*60
# N_students
p_app_d = 0.0
# p_tested
p_test_high_contact = 0.95
p_test_low_contact = 0.95
# p_traced
p_mask = 0.0
test_delay_d = 1.0
trace_delay_manual_d = 1.0
trace_delay_app_d = 1.0
manual_tracing_threshld = 0
app_tracing_threshold = 0
mask_reduction_out_d = 0.6
mask_reduction_in_d = 0.9
tracelength_d = 12
quarantine_length = 14
incubation_period = 5.2
prodromal_period = 1.5
# p_asymptomatic
p_paucisymptomatic = 0.0
# p_mildsymptomatic
# p_severesymptomatic
infectious_period = 7.0
# p_transmission
low_risk_adjustment = 0.85

graph_gamma_shape = 1.65

def write_parameters(filename, *vals):
    textfile = open(filename, "w")
    for v in vals:
        textfile.write(str(v) + "\n")
    textfile.close()

def write_graph_parameters(*vals):
    write_parameters("simulation_single_run/params/graph_params.txt", *vals)

def write_simulation_parameters(*vals):
    write_parameters("simulation_single_run/params/simulation_params.txt", *vals)

def clear_dir(path_to_dir):
    for folder in os.listdir(path_to_dir):
        if folder != '.gitignore':
            shutil.rmtree(os.path.join(path_to_dir, folder))

def run_multiple_simulation():
    N_students_list = [5000, 10000, 20000, 40000]
    graph_gamma_scale_list = [0.3, 0.45, 0.61, 1.21]
    p_tested_list = [0.10, 0.25, 0.40 ,0.50]
    p_traced_list = [0.10, 0.25, 0.40, 0.50, 0.60]
    p_symptom_list = [(0.9, 0.1, 0.0), (0.9, 0.07, 0.03), (0.8, 0.15, 0.05), (0.7, 0.25, 0.05), (0.6, 0.35, 0.05)]
    p_transmission_list = [0.05, 0.10, 0.15]
    n_sim = 20

    N_students_list = [500]
    graph_gamma_scale_list = [0.61]
    p_tested_list = [0.40]
    p_traced_list = [0.60]
    p_symptom_list = [(0.85, 0.12, 0.03)]
    p_transmission_list = [0.10]
    n_sim = 2

    count = 0
    clear_dir("multiple_outputs")
    for N_students in N_students_list:
        for graph_gamma_scale in graph_gamma_scale_list:
            for p_tested in p_tested_list:
                for p_traced in p_traced_list:
                    for p_asymptomatic, p_mildsymptomatic, p_severesymptomatic in p_symptom_list:
                        for p_transmission in p_transmission_list:
                            write_graph_parameters(N_students, graph_gamma_shape, graph_gamma_scale)
                            write_simulation_parameters(
                                N_students,
                                p_app_d,
                                p_tested,
                                p_test_high_contact,
                                p_test_low_contact,
                                p_traced,
                                p_mask,
                                test_delay_d,
                                trace_delay_manual_d,
                                trace_delay_app_d,
                                manual_tracing_threshld,
                                app_tracing_threshold,
                                mask_reduction_out_d,
                                mask_reduction_in_d,
                                tracelength_d,
                                quarantine_length,
                                incubation_period,
                                prodromal_period,
                                p_asymptomatic,
                                p_paucisymptomatic,
                                p_mildsymptomatic,
                                p_severesymptomatic,
                                infectious_period,
                                p_transmission,
                                low_risk_adjustment
                                )
                            count += 1
                            parent_dir = os.path.join("multiple_outputs/", "parameters_" + str(count))
                            source_dir = os.path.join("simulation_single_run/", "outputs")
                            os.mkdir(parent_dir)
                            for sim in range(1, n_sim+1):
                                # with open("simulation_single_run/run.sh", "rb") as file:
                                #     script = file.read()
                                # rc = call(script)
                                rc = call("simulation_single_run/run.sh")
                                # Move output to appropriate folder
                                final_dir_name = os.path.join(parent_dir, "simulation_" + str(sim))
                                shutil.copytree(source_dir, final_dir_name)
                                # os.rename(os.path.join(parent_dir, "outputs"), final_dir_name)
                            shutil.copy2("simulation_single_run/params/simulation_params.txt", parent_dir)
                            shutil.copy2("simulation_single_run/params/graph_params.txt", parent_dir)


if __name__=='__main__':
    run_multiple_simulation()

