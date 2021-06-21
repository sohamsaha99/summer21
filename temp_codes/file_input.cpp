#include <bits/stdc++.h>
using namespace std;

const int day=24*60*60;
const int timestep_in_data=300;

float p_app_d=0.0;
float p_tested=0.00;
float p_test_high_contact=0.95;
float p_test_low_contact=0.80;
float p_traced=0.0;
float p_mask=0.00;
float test_delay_d=1.0*day;
float trace_delay_manual_d=1.0*day;
float trace_delay_app_d=0.0;
int manual_tracing_threshold=0;
int app_tracing_threshold=0;
float mask_reduction_out_d=0.6;
float mask_reduction_in_d=0.9;
int tracelength_d = 6*day;
int quarantine_length=14*day;
float incubation_period=5.2*day;
float prodromal_period=1.5*day;
float latency_period=incubation_period - prodromal_period;
float p_asymptomatic=0.60;
float p_paucisymptomatic=0.25*(1 - p_asymptomatic);
float p_mildsymptomatic=0.75*(1 - p_asymptomatic);
float p_severesymptomatic=0.00*(1 - p_asymptomatic);
// float infectious_period=7.5*day - incubation_period;
float infectious_period=9.5*day - incubation_period;

float p_transmission=0.3; ////////////////////////////////////////////////////////////////////////////////////////////
float low_risk_adjustment=0.7;////////////////////////////////////////////////////////////////////////////////////////

void __attribute__((optimize("O0"))) takeInputFromFile(const char *filename)
{
    if(FILE *fp=fopen(filename, "r"))
    {
        cout<<"Opened File\n";
        char *nothing;
        int a,d;
        float x;
        a=fscanf(fp, "%f", &x); p_app_d=x;
        a=fscanf(fp, "%f", &x); p_tested=x;
        a=fscanf(fp, "%f", &x); p_test_high_contact=x;
        a=fscanf(fp, "%f", &x); p_test_low_contact=x;
        a=fscanf(fp, "%f", &x); p_traced=x;
        a=fscanf(fp, "%f", &x); p_mask=x;
        a=fscanf(fp, "%f", &x); test_delay_d=x*day;
        a=fscanf(fp, "%f", &x); trace_delay_manual_d=x*day;
        a=fscanf(fp, "%f", &x); trace_delay_app_d=x*day;
        a=fscanf(fp, "%d", &d); manual_tracing_threshold=d;
        a=fscanf(fp, "%d", &d); app_tracing_threshold=d;
        a=fscanf(fp, "%f", &x); mask_reduction_out_d=x;
        a=fscanf(fp, "%f", &x); mask_reduction_in_d=x;
        a=fscanf(fp, "%d", &d); tracelength_d=d*day;
        a=fscanf(fp, "%d", &d); quarantine_length=d*day;
        a=fscanf(fp, "%f", &x); incubation_period=x*day;
        a=fscanf(fp, "%f", &x); prodromal_period=x*day;
        a=fscanf(fp, "%f", &x); p_asymptomatic=x;
        a=fscanf(fp, "%f", &x); p_paucisymptomatic=x;
        a=fscanf(fp, "%f", &x); p_mildsymptomatic=x;
        a=fscanf(fp, "%f", &x); p_severesymptomatic=x;
        a=fscanf(fp, "%f", &x); infectious_period=x*day;
        a=fscanf(fp, "%f", &x); p_transmission=x;
        a=fscanf(fp, "%f", &x); low_risk_adjustment=x;
        fclose(fp);
        printf("p_app_d: %f\n"
                "p_tested: %f\n"
                "p_test_high_contact: %f\n"
                "p_test_low_contact: %f",
                p_app_d, p_tested, p_test_high_contact, p_test_low_contact);
    }
}
int main(int argc, const char *argv[])
{
    if(argc>1)
    {
        takeInputFromFile(argv[1]);
    }
    // PRINT PARAMETERS:
    printf("p_app_d: %f\n"
            "p_tested: %f\n"
            "p_test_high_contact: %f\n"
            "p_test_low_contact: %f",
            p_app_d, p_tested, p_test_high_contact, p_test_low_contact);
    printf("%f, %f, %f, %d, %d, %f\n", p_tested, p_traced, trace_delay_manual_d, manual_tracing_threshold, quarantine_length, low_risk_adjustment);
    return 0;
}