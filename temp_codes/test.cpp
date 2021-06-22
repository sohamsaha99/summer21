#include <bits/stdc++.h>
using namespace std;
float b=10.0;
int c=-1;
random_device rd;
mt19937 gen(rd());

float trace_delay_app_d = 5.0;
float p_asymptomatic=1.0;
float p_paucisymptomatic=0.25*(1 - p_asymptomatic);
float p_mildsymptomatic=0.75*(1 - p_asymptomatic);
float p_severesymptomatic=0.00*(1 - p_asymptomatic);
normal_distribution<float> rnorm_trace_delay_app(trace_delay_app_d, 0.25);
discrete_distribution<int> sample_I_class={p_asymptomatic, p_paucisymptomatic, p_mildsymptomatic, p_severesymptomatic};
char Iclass[] = {'J', 'K', 'L', 'M'};

int main(int argc, char const *argv[])
{
    char I=Iclass[sample_I_class(gen)];
    cout<<I<<"\n";
    cout<<rnorm_trace_delay_app(gen)<<"\n";
    trace_delay_app_d /= 2.0;
    rnorm_trace_delay_app = normal_distribution<float>(trace_delay_app_d, 0.25);
    sample_I_class = discrete_distribution<int>({0.0, 0.0, 0.5, 0.5});
    cout<<rnorm_trace_delay_app(gen)<<"\n";
    I=Iclass[sample_I_class(gen)];
    cout<<I<<"\n";
    return 0;
}