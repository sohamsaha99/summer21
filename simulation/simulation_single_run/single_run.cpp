/////////////////////////////////////////////////////////////////////
//////          DONT MODIFY N_students inside mai   /////////////////
//////          NOW AVAIL;ABLE IN GLOBAL            /////////////////
/////////////////////////////////////////////////////////////////////
#define ROUND_2_INT(f) ((int)((f) >= 0.0 ? ((f) + 0.5) : ((f) - 0.5)))
#define LOG_NORMAL_FIND_UNDERLYING_MEAN(mu, sgm) (log((mu) /sqrt(1 + ((sgm) * (sgm) / ((mu) * (mu))))))
#define LOG_NORMAL_FIND_UNDERLYING_SIGMA(mu, sgm) (sqrt(log(1 + ((sgm) * (sgm) / ((mu) * (mu))))))
// #define ROUND_2_INT(f) ((int)((f) >= 0.0 ? ((f) + 0.5) : (1.0)))

#include <bits/stdc++.h>
#include <chrono>
using namespace std;

const int day=24*60*60;
const int timestep_in_data=300;
int N_students=5000;
float p_app_d=0.0;
float p_tested=0.40;
float p_test_high_contact=0.95;
float p_test_low_contact=0.95;
float p_traced=0.5;
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
float p_asymptomatic=0.90;
float p_paucisymptomatic=0.0*(1 - p_asymptomatic);
float p_mildsymptomatic=1.0*(1 - p_asymptomatic);
float p_severesymptomatic=0.00*(1 - p_asymptomatic);
// float infectious_period=7.5*day - incubation_period;
float infectious_period=9.5*day - incubation_period;

float p_transmission=0.10; ////////////////////////////////////////////////////////////////////////////////////////////
float low_risk_adjustment=1.0;////////////////////////////////////////////////////////////////////////////////////////

random_device rd;
mt19937 gen(rd());
// default_random_engine gen;
lognormal_distribution<float> rnorm_latency_period(LOG_NORMAL_FIND_UNDERLYING_MEAN(latency_period, (latency_period/10.0)), LOG_NORMAL_FIND_UNDERLYING_SIGMA(latency_period, (latency_period/10.0)));
lognormal_distribution<float> rnorm_prodromal_period(LOG_NORMAL_FIND_UNDERLYING_MEAN(prodromal_period, (prodromal_period/10.0)), LOG_NORMAL_FIND_UNDERLYING_SIGMA(prodromal_period, (prodromal_period/10.0)));
normal_distribution<float> rnorm_test_delay(test_delay_d, test_delay_d/10.0);
lognormal_distribution<float> rnorm_infectious_period(LOG_NORMAL_FIND_UNDERLYING_MEAN(infectious_period, (infectious_period/10.0)), LOG_NORMAL_FIND_UNDERLYING_SIGMA(infectious_period, (infectious_period/10.0)));
normal_distribution<float> rnorm_trace_delay_manual(trace_delay_manual_d, trace_delay_manual_d/10.0);
normal_distribution<float> rnorm_trace_delay_app(trace_delay_app_d, trace_delay_app_d/10.0);
discrete_distribution<int> sample_I_class={p_asymptomatic, p_paucisymptomatic, p_mildsymptomatic, p_severesymptomatic};
// discrete_distribution<int> sample_I_class={1, 2, 3, 4};
char Iclass[] = {'J', 'K', 'L', 'M'};
vector<pair<int, char>> BLANKEVENT={};

// Print Event Queue
void showEvents(const vector<vector<pair<int, char>>> &eventq)
{
    // vector<vector<pair<int, int>>>::iterator it;
    // vector<pair<int, int>>::iterator jt;
    for(int i=0; i<eventq.size(); ++i)
    {
        // cout<<"Printing Events:"<<eventq[i].size()<<"\n";
        for(int j=0; j<eventq[i].size(); ++j)
        {
            cout<<"At time "<<i<<", we have an event for P"<<get<0>(eventq[i][j])<<" titled "<<get<1>(eventq[i][j])<<"\n";
        }
    }
}
// Insert into Event Queue
inline void insertEvent(vector<vector<pair<int, char>>> &eventq, int currtime, pair<int, char> event)
{
    while(eventq.size() <= currtime)
    {
        eventq.push_back(BLANKEVENT);
    }
    eventq[currtime].push_back(event);
}
void insertContactEvent(vector<vector<tuple<int, int, bool>>> &contactdict, int timestamp, tuple<int, int, bool> event)
{
    // vector<tuple<int, int, bool>> BLANK={};
    while(contactdict.size()<=timestamp)
    {
        // contactdict.push_back(BLANK);
        contactdict.push_back({});
    }
    contactdict[timestamp].push_back(event);
}
void parseCSV(string filename, vector<vector<tuple<int, int, bool>>> &contactdict, unordered_set<int> &student_ids)
{
    std::ifstream in(filename);
    std::string line;
    std::getline(in, line);
    while(std::getline(in, line))
    {
        std::stringstream ss(line);
        std::tuple<int, int, int, float> row;
        std::string data;
        std::getline(ss, data, ',');
        // cout<<data<<"\n";
        int timestamp=stoi(data)/timestep_in_data;
        std::getline(ss, data, ',');
        int P1=stoi(data);
        std::getline(ss, data, ',');
        int P2=stoi(data);
        std::getline(ss, data, ',');
        float strength=stod(data);
        // cout<<timestamp<<" "<<P1<<" "<<" "<<P2<<" "<<strength<<"\n";
        if(strength!=0.0)
        {
            insertContactEvent(contactdict, timestamp, {P1, P2, true});
            student_ids.insert(P1);
            student_ids.insert(P2);
        }
    }
}
int parseCSVFast(string filename, vector<vector<tuple<int, int, bool>>> &contactdict, unordered_set<int> &student_ids)
{
    int timestamp, P1, P2, k=0;
    float strength;
    if(FILE *fp=fopen(filename.c_str(), "r"))
    {
        // P1=fscanf(fp, "%*[^\n]\n");
        fscanf(fp, "%*[^\n]\n");
        while(fscanf(fp, "%d,%d,%d,%f", &timestamp, &P1, &P2, &strength)==4)
        {
            k++;
            if(strength > 0.5)
            {
                insertContactEvent(contactdict, timestamp/timestep_in_data, {P1, P2, true});
            }
            else
            {
                insertContactEvent(contactdict, timestamp/timestep_in_data, {P1, P2, false});
            }
            student_ids.insert(P1);
            student_ids.insert(P2);
        }
        fclose(fp);
    }
    return k;
}
void generateContactEvents(string household_edges_file, string outer_edges_file, vector<vector<tuple<int, int, bool>>> &contactdict, vector<pair<int, int>> &household_edges, vector<pair<int, int>> &outer_edges, string &household_edges_file_prev, string &outer_edges_file_prev)
{
    if(household_edges_file_prev!=household_edges_file)
    {
        // printf("Got new file for family.\n");
        // Read household_edges_file and insert in household_edges
        int P1, P2, k=0;
        if(FILE *fp=fopen(household_edges_file.c_str(), "r"))
        {
            household_edges.clear();
            while(fscanf(fp, "%d,%d",&P1, &P2)==2)
            {
                if(P1<=N_students && P2<=N_students)
                {
                    k++;
                    household_edges.push_back({P1, P2});
                }
            }
            fclose(fp);
            household_edges_file_prev=household_edges_file;
        }
    }
    if(outer_edges_file_prev!=outer_edges_file)
    {
        // printf("Got new file for outer.\n");
        // Read outer_edges_file and insert in outer_edges
        int P1, P2, k=0;
        if(FILE *fp=fopen(outer_edges_file.c_str(), "r"))
        {
            outer_edges.clear();
            while(fscanf(fp, "%d,%d",&P1, &P2)==2)
            {
                if(P1<=N_students && P2<=N_students)
                {
                    k++;
                    outer_edges.push_back({P1, P2});
                }
            }
            fclose(fp);
            outer_edges_file_prev=outer_edges_file;
        }
    }
    contactdict.clear();
    for(int i=0; i<household_edges.size(); i++)
    {
        int t=rand()%(day/timestep_in_data)+1;
        insertContactEvent(contactdict, t, {get<0>(household_edges[i]), get<1>(household_edges[i]), true});
    }
    for(int i=0; i<outer_edges.size(); i++)
    {
        int t=rand()%(day/timestep_in_data)+1;
        if(static_cast<float>(rand())/static_cast<float>(RAND_MAX) < 0.9)
        {
            insertContactEvent(contactdict, t, {get<0>(outer_edges[i]), get<1>(outer_edges[i]), false});
        }
        else
        {
            insertContactEvent(contactdict, t, {get<0>(outer_edges[i]), get<1>(outer_edges[i]), true});
        }
    }
}
void showContactEvents(const vector<vector<tuple<int, int, bool>>> &contactdict)
{
    for(int i=0; i<contactdict.size(); i++)
    {
        for(int j=0; j<contactdict[i].size(); j++)
        {
            cout<<"At time "<<i<<", contact between P"<<get<0>(contactdict[i][j])<<" and P"<<get<1>(contactdict[i][j])<<"\n";
        }
    }
}
void showStudentIds(unordered_set<int> &student_ids)
{
    for(auto it=student_ids.begin(); it!=student_ids.end(); it++)
    {
        cout<<(*it)<<" ";
    }
    cout<<"\n";
}
void dumpCSVtotalCounts(string filename, const vector<tuple<int, int, int, int>> &totalCounts)
{
    int S, E, I, R;
    tuple<int, int, int, int> counts;
    if(FILE *fp=fopen(filename.c_str(), "w"))
    {
        // P1=fscanf(fp, "%*[^\n]\n");
        fprintf(fp, "day, S, E, I, R\n");
        for(int i=0; i<totalCounts.size(); i++)
        {
            counts=totalCounts[i];
            fprintf(fp, "%d, %d, %d, %d, %d\n", i, get<0>(counts), get<1>(counts), get<2>(counts), get<3>(counts));
        }
        fclose(fp);
    }
}
void dumpJSONtransmissionChain(string filename, const vector<tuple<int, int, char, int, char>> &transmissionChain)
{
    unordered_map<int, char> types;
    unordered_set<int> student_ids;
    tuple<int, int, char, int, char> link;
    for(int i=0; i<transmissionChain.size(); i++)
    {
        link=transmissionChain[i];
        student_ids.insert(get<1>(link));
        types[get<1>(link)] = get<2>(link);
        student_ids.insert(get<3>(link));
        types[get<3>(link)] = get<4>(link);
    }
    if(FILE *fp = fopen(filename.c_str(), "w"))
    {
        fprintf(fp, "{\n");
        fprintf(fp, "    \"nodes\": [\n");
        string group;
        int k=0, l=student_ids.size();
        for(auto it=student_ids.begin(); it!=student_ids.end(); ++it)
        {
            k++;
            fprintf(fp, "        {\n");
            fprintf(fp, "            \"name\": \"%d\",\n", *it);
            fprintf(fp, "            \"id\": %d,\n", *it);
            if(types[*it] == 'S')
            {
                group="green";
            }
            else if(types[*it] == 'E')
            {
                group="yellow";
            }
            else if(types[*it] == 'R')
            {
                group="red";
            }
            else
            {
                group="blue";
            }
            fprintf(fp, "            \"group\": \"%s\"\n", group.c_str());
            if(k < l)
            {
                fprintf(fp, "        },\n");
            }
            else
            {
                fprintf(fp, "        }\n");
            }
        }
        fprintf(fp, "    ],\n");
        fprintf(fp, "    \"links\": [\n");
        k=0;
        l=transmissionChain.size();
        for(int i=0; i<transmissionChain.size(); i++)
        {
            k++;
            fprintf(fp, "        {\n");
            fprintf(fp, "            \"source\": %d,\n", get<1>(transmissionChain[i]));
            fprintf(fp, "            \"target\": %d,\n", get<3>(transmissionChain[i]));
            fprintf(fp, "            \"state\": \"%c\",\n", get<2>(transmissionChain[i]));
            fprintf(fp, "            \"type\": \"%.2f\"\n", get<0>(transmissionChain[i])*timestep_in_data/(float)(day));
            if(k < l)
            {
                fprintf(fp, "        },\n");
            }
            else
            {
                fprintf(fp, "        }\n");
            }
        }
        fprintf(fp, "    ]\n");
        fprintf(fp, "}\n");
        fclose(fp);
    }
}
void dumpUsefulContacts(string filename, const vector<tuple<int, int, char, int, char>> &usefulContacts)
{
    if(FILE *fp=fopen(filename.c_str(), "w"))
    {
        tuple<int, int, char, int, char> contact;
        fprintf(fp, "time, P1, S1, P2, S2\n");
        for(int i=0; i<usefulContacts.size(); i++)
        {
            contact=usefulContacts[i];
            fprintf(fp, "%.2f, %d, %c, %d, %c\n", get<0>(contact)*timestep_in_data/(float)(day), get<1>(contact), get<2>(contact), get<3>(contact), get<4>(contact));
        }
        fclose(fp);
    }
}
class nodeClass
{
public:
    static vector<tuple<int, int, char, bool, int, int>> official_tests_data; // id, collection time, reason(K, L, M for symptoms. T for tracing) result(True: Positive), result time, test Id
    static vector<tuple<int, int, int, bool, bool, int>> official_tracing_data; // source, target, tracing time, sample collected, High/low risk contact(True: high), test Id
    static int testId; // Increment by 1 for each new test
    static unordered_map<int, int> ongoing_tests; // Ongoing tests patient id, test id
    static vector<bool> positive_patients;
    static vector<tuple<int, char, int, int, int, int>> symptoms; // id, symptom, exposure.time, presympt.time, sympt.time, removed.time
    static vector<tuple<int, int, char>> quarantine_records; // id, time, event ('B' for begin, 'E' for end)
    char state;     // S, E, Ip,Ias,Ips,Ims, Iss, R (Denoted here by S, E, I, J, K, L, M, R)
    // char substate;  // Mainly for state I: L for presympt, P - Pauci, A - Asy, M - Mild, S - Severe, if state is not I, set to X
    bool infectious;
    float dampingfactor;
    deque<tuple<int, int, bool>> contacts;   // Queue of contacts, each entry is tuple of (time, id, bool high_risk)
    unordered_map<int, int> count_of_contacts; // Update this whenever the 'contacts' deque changes
    unordered_map<int, int> count_of_high_risk_contacts; // Update this in add_contacts anf flush_contacts
    set<int> contact_has_app;
    bool in_quarantine;
    int id;
    bool has_mask;
    float mask_factor_out;
    float mask_factor_in;
    bool has_app;
    bool awaiting_result;
    bool testedPositive; // set to true if tested positive sometime (so that we don't test again and again the same person even after receiving a positive result)
    int quarantine_release_time; // set to -1 if the person is not in quarantine, else set to end of quarantine time.
    nodeClass(int myid, float param_p_app=p_app_d, float param_p_mask=p_mask, float param_mask_reduction_out=mask_reduction_out_d, float param_mask_reduction_in=mask_reduction_in_d)
    {
        state='S';
        // substate='X';
        infectious=false;
        dampingfactor=1.0;
        contacts.clear();
        contact_has_app.clear();
        count_of_contacts.clear();
        count_of_high_risk_contacts.clear();
        in_quarantine=false;
        awaiting_result=false;
        testedPositive=false;
        quarantine_release_time=-1;
        id=myid;
        if(static_cast <float>(rand())/static_cast <float>(RAND_MAX) < param_p_mask)
        {
            has_mask=true;
            mask_factor_out=param_mask_reduction_out;
            mask_factor_in=param_mask_reduction_in;
        }
        else
        {
            has_mask=false;
            mask_factor_out=1.0;
            mask_factor_in=1.0;
        }
        if(static_cast <float>(rand())/static_cast <float>(RAND_MAX) < param_p_app)
        {
            has_app=true;
        }
        else
        {
            has_app=false;
        }
    }
    // ~nodeClass();
    void reset(int myid)
    {
        state='S';
        // substate='X';
        infectious=false;
        dampingfactor=1.0;
        contacts.clear();
        count_of_contacts.clear();
        count_of_high_risk_contacts.clear();
        contact_has_app.clear();
        in_quarantine=false;
        awaiting_result=false;
        testedPositive=false;
        id=myid;
    }
    void statechange(vector<vector<pair<int, char>>> &eventq, char newstate, int currtime, unordered_set<int> &students_with_apps, float param_p_traced=p_traced, float param_p_tested=p_tested, int param_tracelength=tracelength_d/timestep_in_data, int param_quarantine_length=quarantine_length/timestep_in_data, int param_manual_tracing_threshold=manual_tracing_threshold, int param_app_tracing_threshold=app_tracing_threshold, float param_p_test_high_contact=p_test_high_contact, float param_p_test_low_contact=p_test_low_contact)//, set<int> students_with_apps)// SHOULDSETPARAMS
    {
        if(newstate=='F')// END OF QUARANTINE
        {
            if(in_quarantine & quarantine_release_time==currtime)
            {
                quarantine_records.push_back({id, currtime, 'E'}); // E: End of quarantine
                in_quarantine=false;
                quarantine_release_time=-1;
            }
        }
        else if(newstate=='B' || newstate=='C')//B: Begin Quarantine, C: Begin Quarantine due to tracing
        {
            if(!in_quarantine)
            {
                set_quarantine(eventq, currtime, param_quarantine_length);
                // cout<<"Should send P"<<id<<" to quarantine\n";
            }
        }
        else if(newstate=='T')
        {
            trace_contacts(eventq, currtime, students_with_apps, param_p_traced, param_p_test_high_contact, param_p_test_low_contact, param_tracelength, param_manual_tracing_threshold, param_app_tracing_threshold);
            // cout<<"Tracing begins for P"<<id<<" at time "<<currtime<<"\n";
        }
        else if(newstate=='G')
        {
            if(!awaiting_result && !testedPositive)
            {
                // cout<<"Testing for P"<<id<<"\n";
                covid_test(eventq, currtime, 'Z');
            }
            // else
            // {
            //     cout<<"DON'T TEST. ALREADY DONE. P"<<id<<"\n";
            // }
        }
        else if(newstate=='N')
        {
            awaiting_result=false;
            ongoing_tests.erase(id);
            if(in_quarantine) // Un-quarantine due to negative result. So don't check for quarantine_release_time
            {
                quarantine_records.push_back({id, currtime, 'E'});
            }
            in_quarantine=false; // Negative people are released.
            quarantine_release_time=-1;
        }
        else
        {
            state=newstate;
            if(state=='I'||state=='J'||state=='K'||state=='L'||state=='M')
            {
                infectious=true;
            }
            else
            {
                infectious=false;
            }
            if(!(state=='I' || state=='J' || state=='K' || state=='L' || state=='M' || state=='R'))
            {
                cout<<"ERROR! State="<<state<<"\n";
            }
            if(infectious && newstate != 'I' && newstate != 'J' && !awaiting_result && !testedPositive && (newstate=='M' || static_cast <float>(rand())/static_cast <float>(RAND_MAX) < param_p_tested))
            {
                int time_to_testing=covid_test(eventq, currtime, state);
                if(!in_quarantine)
                {
                    insertEvent(eventq, time_to_testing, {id, 'B'});
                }
            }
        }
    }
    void exposure(vector<vector<pair<int, char>>> &eventq, int currtime)// SHOULDSETPARAMS
    {
        state='E';
        float temp=0.0;
        temp=((currtime*timestep_in_data)+rnorm_latency_period(gen))/timestep_in_data;
        int time_to_ip=ROUND_2_INT(temp);
        insertEvent(eventq, time_to_ip, {id, 'I'});
        temp=((time_to_ip*timestep_in_data)+rnorm_prodromal_period(gen))/timestep_in_data;
        int time_to_i=ROUND_2_INT(temp);
        char I=Iclass[sample_I_class(gen)];
        insertEvent(eventq, time_to_i, {id, I});
        if(I=='M')
        {
            dampingfactor=1.0;
        }
        else
        {
            dampingfactor=1.0;
        }
        // cout<<id<<I<<" ";
        // if(I != 'J')
        // {
        //     if(I=='M' || static_cast <float>(rand())/static_cast <float>(RAND_MAX) < param_p_tested)
        //     {
        //         int time_to_testing=covid_test(eventq, time_to_i, I);
                // cout<<"HELLOOOOOOOOOOOOOOOO\n";
                // temp=((time_to_i*timestep_in_data)+rnorm_test_delay(gen))/timestep_in_data;
                // int time_to_testing=ROUND_2_INT(temp);
                // if(!in_quarantine)
                // {
                //     insertEvent(eventq, time_to_testing, {id, 'B'});
                // }
                // insertEvent(eventq, time_to_testing, {id, 'B'});
                // insertEvent(eventq, time_to_testing+1, {id, 'T'});
            // }
        // }
        temp=((time_to_i*timestep_in_data)+rnorm_infectious_period(gen))/timestep_in_data;
        int time_to_r=ROUND_2_INT(temp);
        insertEvent(eventq, time_to_r, {id, 'R'});
        symptoms.push_back({id, I, currtime, time_to_ip, time_to_i, time_to_r});
    }
    void add_contact(int student_id, int currtime, bool high_risk)
    {
        contacts.push_back({currtime, student_id, high_risk});
        count_of_contacts[student_id] += 1;
        if(high_risk)
        {
            count_of_high_risk_contacts[student_id] += 1;
        }
    }
    void flush_contacts(int currtime, int param_tracelength=tracelength_d/timestep_in_data)
    {
        tuple<int, int, bool> time_sid_strength;
        while(!contacts.empty())
        {
            // cout<<currtime-get<0>(contacts.front())<<param_tracelength;
            time_sid_strength=contacts.front();
            if(currtime-get<0>(time_sid_strength) > param_tracelength)
            {
                count_of_contacts[get<1>(time_sid_strength)] -= 1;
                if(get<2>(time_sid_strength))
                {
                    count_of_high_risk_contacts[get<1>(time_sid_strength)] -= 1;
                }
                contacts.pop_front();
            }
            else
            {
                break;
            }
        }
        for(auto it=count_of_contacts.begin(); it!=count_of_contacts.end();)
        {
            if(it->second == 0)
            {
                // cout<<"Erasing for "<<it->first<<"\n";
                it = count_of_contacts.erase(it);
            }
            else
            {
                // cout<<"Not erasing for "<<it->first<<", count="<<it->second<<"\n";
                ++it;
            }
        }
        for(auto it=count_of_high_risk_contacts.begin(); it!=count_of_high_risk_contacts.end();)
        {
            if(it->second == 0)
            {
                // cout<<"Erasing for "<<it->first<<"\n";
                it = count_of_high_risk_contacts.erase(it);
            }
            else
            {
                // cout<<"Not erasing for "<<it->first<<", count="<<it->second<<"\n";
                ++it;
            }
        }
    }
    void trace_contacts(vector<vector<pair<int, char>>> &eventq, int currtime, unordered_set<int> &students_with_apps, float param_p_traced=p_traced, float param_p_test_high_contact=p_test_high_contact, float param_p_test_low_contact=p_test_low_contact, int param_tracelength=tracelength_d/timestep_in_data, int param_manual_tracing_threshold=manual_tracing_threshold, int param_app_tracing_threshold=app_tracing_threshold)
    {
        awaiting_result=false;
        testedPositive=true;
        ongoing_tests.erase(id);
        positive_patients[id]=true;
        float temp=0.0;
        flush_contacts(currtime, param_tracelength);
        for(auto& kv:count_of_contacts)
        {
            bool put_in_quarantine=false;
            int quarantine_time;
            if(kv.second > param_manual_tracing_threshold)
            {
                if(static_cast <float>(rand())/static_cast <float>(RAND_MAX) < param_p_traced)// ((float)(kv.second))/(kv.second + 12.0)) // NOT USING param_p_traced
                {
                    put_in_quarantine=true;
                    temp=((currtime*timestep_in_data)+rnorm_trace_delay_manual(gen))/timestep_in_data;
                    quarantine_time=ROUND_2_INT(temp);
                }
            }
            if(has_app && kv.second > param_app_tracing_threshold && students_with_apps.find(kv.first) != students_with_apps.end())
            {
                put_in_quarantine=true;
                temp=((currtime*timestep_in_data)+rnorm_trace_delay_app(gen))/timestep_in_data;
                quarantine_time=ROUND_2_INT(temp);
            }
            if(put_in_quarantine)
            {
                insertEvent(eventq, quarantine_time, {kv.first, 'C'});
                if(count_of_high_risk_contacts.count(kv.first))
                {
                    // THE CONTACT WAS HIGH RISK, AND RECALLED
                    if(static_cast <float>(rand())/static_cast <float>(RAND_MAX) < param_p_test_high_contact)
                    {
                        if(ongoing_tests.count(kv.first)==0)
                        {
                            if(positive_patients[kv.first])
                            {
                                official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, false, true, -1));
                            }
                            else
                            {
                                insertEvent(eventq, quarantine_time, {kv.first, 'G'}); // G for testinG
                                official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, true, true, ++testId)); //SAMPLE COLLECTED, HIGH RISK
                                ongoing_tests[kv.first] = testId;
                            }
                        }
                        else
                        {
                            official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, true, true, ongoing_tests[kv.first])); //SAMPLE COLLECTED, HIGH RISK
                        }
                        // cout<<"HIGH RISK "<<currtime<<" contact of "<<id<<"\n";
                    }
                    else
                    {
                        official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, false, true, 0)); //SAMPLE NOT COLLECTED, HIGH RISK
                    }
                }
                else
                {
                    // LOW RISK, AND RECALLED
                    if(static_cast <float>(rand())/static_cast <float>(RAND_MAX) < param_p_test_low_contact)
                    {
                        if(ongoing_tests.count(kv.first)==0)
                        {
                            if(positive_patients[kv.first]) // Contact already tested some time and received positive. So no point of testing now
                            {
                                official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, false, false, -1));
                            }
                            else
                            {
                                insertEvent(eventq, quarantine_time, {kv.first, 'G'});
                                official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, true, false, ++testId));
                                ongoing_tests[kv.first] = testId;
                            }
                        }
                        else
                        {
                            official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, true, false, ongoing_tests[kv.first]));
                        }
                        // cout<<"LOW RISK "<<currtime<<" contact of "<<id<<"\n";
                    }
                    else
                    {
                        official_tracing_data.push_back(make_tuple(id, kv.first, quarantine_time, false, false, 0));
                    }
                }
            }
        }        
    }
    int covid_test(vector<vector<pair<int, char>>> &eventq, int currtime, char reason) // DOES NOT HANDLE QUARANTINES
    {
        float temp=0.0;
        temp = ((currtime*timestep_in_data)+rnorm_test_delay(gen))/timestep_in_data;
        int time_to_result = ROUND_2_INT(temp);
        if(ongoing_tests.count(id)==0)
        {
            testId++;
            ongoing_tests[id] = testId;
        }
        awaiting_result=true;
        if(state=='E' || state=='I' || state=='J' || state=='K' || state=='L' || state=='M') // WARNING: EXPOSED ARE TESTED POSITIVE?
        {
            // IN THESE CASES, THE RESULT WILL BE POSITIVE
            insertEvent(eventq, time_to_result, {id, 'T'});
            official_tests_data.push_back(make_tuple(id, currtime, reason, true, time_to_result, ongoing_tests[id]));
        }
        else
        {
            insertEvent(eventq, time_to_result, {id, 'N'});
            official_tests_data.push_back(make_tuple(id, currtime, reason, false, time_to_result, ongoing_tests[id]));
        }
        return time_to_result;
    }
    // void arrival_of_negative_result()
    // {
    //     awaiting_result=false;
    // }
    void set_quarantine(vector<vector<pair<int, char>>> &eventq, int currtime, int param_quarantine_length=quarantine_length/timestep_in_data)
    {
        if(!in_quarantine)
        {
            int eoq_time=currtime+param_quarantine_length;
            insertEvent(eventq, eoq_time, {id, 'F'});
            quarantine_records.push_back({id, currtime, 'B'});
            in_quarantine=true;
            quarantine_release_time=eoq_time;
        }
    }
    void printnode(int currtime=0)
    {
        cout<<"Printing P"<<id<<" at time "<<currtime<<":\n";
        cout<<"State: "<<state<<", Infectious? "<<infectious<<", DampingFactor="<<dampingfactor<<", In Q? "<<in_quarantine<<"Has mask? "<<has_mask<<mask_factor_out<<mask_factor_in<<has_app<<"\n";
        deque<tuple<int, int, bool>>::iterator g;
        for (g = contacts.begin(); g != contacts.end(); ++g)
        {
            cout<<"Contact with P"<<get<1>(*g)<<" at time "<<get<0>(*g)<<"\n";
        }
        for(auto& kv:count_of_contacts)
        {
            cout<<"Contact happened with P"<<kv.first<<" "<<kv.second<<" times.\n";
        }
    }
    static void dumpOfficialTestsData(string filename)
    {
        if(FILE *fp=fopen(filename.c_str(), "w"))
        {
            fprintf(fp, "id, collection.time, reason, result, result.date, test.id\n");
            tuple<int, int, char, bool, int, int> test_event;
            char reason;
            string reasonString, result;
            for(int i=0; i<official_tests_data.size(); i++)
            {
                test_event=official_tests_data[i];
                reason=get<2>(test_event);
                if(reason=='K')
                {
                    reasonString="PAUCI";
                }
                else if(reason=='L')
                {
                    reasonString="MILD";
                }
                else if(reason=='M')
                {
                    reasonString="SEVERE";
                }
                else if(reason=='Z')
                {
                    reasonString="TRACING";
                }
                else
                {
                    reasonString="UNKNOWN";
                }
                if(get<3>(test_event))
                {
                    result="POSITIVE";
                }
                else
                {
                    result="NEGATIVE";
                }
                fprintf(fp, "%d, %.2f, %s, %s, %.2f, %d\n", get<0>(test_event), get<1>(test_event)*timestep_in_data/(float)(day), reasonString.c_str(), result.c_str(), get<4>(test_event)*timestep_in_data/(float)(day), get<5>(test_event));
            }
            fclose(fp);
        }
    }
    static void dumpOfficialTracingData(string filename)
    {
        if(FILE *fp=fopen(filename.c_str(), "w"))
        {
            fprintf(fp, "case.id, contact.id, tracing.time, sample.collected, risk.type, test.id\n");
            tuple<int, int, int, bool, bool, int> trace_event;
            string sampleCollected, riskType;
            for(int i=0; i<official_tracing_data.size(); i++)
            {
                trace_event=official_tracing_data[i];
                if(get<3>(trace_event))
                {
                    sampleCollected="YES";
                }
                else
                {
                    sampleCollected="NO";
                }
                if(get<4>(trace_event))
                {
                    riskType="HIGH";
                }
                else
                {
                    riskType="LOW";
                }
                fprintf(fp, "%d, %d, %.2f, %s, %s, %d\n", get<0>(trace_event), get<1>(trace_event), get<2>(trace_event)*timestep_in_data/(float)(day), sampleCollected.c_str(), riskType.c_str(), get<5>(trace_event));
            }
            fclose(fp);
        }
    }
    static void initiatePositiveRecords()
    {
        positive_patients.resize(N_students+1, false);
    }
    static void dumpSymptoms(string filename)
    {
        if(FILE *fp=fopen(filename.c_str(), "w"))
        {
            fprintf(fp, "id, symptom.type, exposure.time, presympt.time, sympt.time, removed.time\n");
            tuple<int, char, int, int, int, int> infection;
            string symptomsString;
            for(int i=0; i<symptoms.size(); i++)
            {
                infection=symptoms[i];
                if(get<1>(infection)=='J')
                {
                    symptomsString="ASYMPTOMATIC";
                }
                else if(get<1>(infection)=='K')
                {
                    symptomsString="PAUCISYMPTOMATIC";
                }
                else if(get<1>(infection)=='L')
                {
                    symptomsString="MILDSYMPTOMATIC";
                }
                else if(get<1>(infection)=='M')
                {
                    symptomsString="SEVERESYMPTOMATIC";
                }
                else
                {
                    symptomsString="ERROR";
                }
                fprintf(fp, "%d, %s, %.2f, %.2f, %.2f, %.2f\n", get<0>(infection), symptomsString.c_str(), get<2>(infection)*timestep_in_data/(float)(day), get<3>(infection)*timestep_in_data/(float)(day), get<4>(infection)*timestep_in_data/(float)(day), get<5>(infection)*timestep_in_data/(float)(day));
            }
            fclose(fp);
        }
    }
    static void dumpQuarantineRecords(string filename)
    {
        if(FILE *fp=fopen(filename.c_str() ,"w"))
        {
            fprintf(fp, "id, time, type\n");
            tuple<int, int, char> quarantine_event;
            for(int i=0; i<quarantine_records.size(); i++)
            {
                quarantine_event=quarantine_records[i];
                fprintf(fp, "%d, %.2f, %c\n", get<0>(quarantine_event), get<1>(quarantine_event)*timestep_in_data/(float)(day), get<2>(quarantine_event));
            }
            fclose(fp);
        }
    }
};
vector<tuple<int, int, char, bool, int, int>> nodeClass::official_tests_data={};
vector<tuple<int, int, int, bool, bool, int>> nodeClass::official_tracing_data={};
int nodeClass::testId=0;
unordered_map<int, int> nodeClass::ongoing_tests={};
vector<bool> nodeClass::positive_patients={};
vector<tuple<int, char, int, int, int, int>> nodeClass::symptoms={};
vector<tuple<int, int, char>> nodeClass::quarantine_records={};
// void simulatePandemic(vector<nodeClass> &studentlist, vector<int> &student_id_list, vector<vector<pair<int, char>>> &eventq, vector<vector<tuple<int, int, bool>>> &contactdict, int curr_time=0, int final_time=-1, vector<tuple<int, int, int>> &transmissionChain, vector<tuple<int, int, int, int>> &totalCounts, bool allSusceptible=true)
// {
//     int first_time;
//     int N_students=student_id_list.size();
//     int initial_period_in_days=7*day/timestep_in_data;
//     if(allSusceptible)
//     {
//         first_time=-1;
//         curr_time=0;
//         int patient_zero_index=student_id_list[rand()%N_students];
//         for(int i=0; i<contactdict.size(); i++)
//         {
//             for(int j=0; j<contactdict[i].size(); j++)
//             {
//                 if(get<0>(contactdict[i][j])==patient_zero_index || get<1>(contactdict[i][j])==patient_zero_index)
//                 {
//                     first_time=i;
//                     break;
//                 }
//             }
//             if(first_time!=-1)
//             {
//                 break;
//             }
//         }
//         curr_time=first_time+rand()%initial_period_in_days;
//         vector<tuple<int, int, bool>> BLANK={};
//         while(contactdict.size()<=curr_time)
//         {
//             contactdict.push_back(BLANK);
//         }
//         max_time=contactdict.size();
//     }
// }
void updateTotalCount(int curr_time, int exposed, int infectious, int total_infected, vector<tuple<int, int, int, int>> &totalCounts)
{
    int day_num=curr_time*timestep_in_data/day;
    // printf("At Day %d, total E=%d\n", day_num, exposed);
    if(totalCounts.size() < day_num)
    {
        totalCounts.insert(totalCounts.end(), day_num - totalCounts.size(), totalCounts.back());
    }
    totalCounts.push_back(make_tuple(N_students - total_infected, exposed, infectious, total_infected - exposed - infectious));
}
void printTotalCount(const vector<tuple<int, int, int, int>> &totalCounts)
{
    tuple<int, int, int, int> counts;
    for(int i=0; i<totalCounts.size(); i++)
    {
        counts=totalCounts[i];
        printf("After Day %3d, S=%3d, E=%3d, I=%3d, R=%3d.\n", i, get<0>(counts), get<1>(counts), get<2>(counts), get<3>(counts));
    }
}
void __attribute__((optimize("O0"))) takeInputFromFile(const char *filename)
{
    if(FILE *fp=fopen(filename, "r"))
    {
        // cout<<"Opened File\n";
        char *nothing;
        int a,d;
        float x;
        a=fscanf(fp, "%d", &d); N_students=d;
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
        // printf("p_app_d: %f\n"
        //         "p_tested: %f\n"
        //         "p_test_high_contact: %f\n"
        //         "p_test_low_contact: %f\n\n",
        //         p_app_d, p_tested, p_test_high_contact, p_test_low_contact);
        latency_period=incubation_period - prodromal_period;
        rnorm_latency_period=lognormal_distribution<float>(LOG_NORMAL_FIND_UNDERLYING_MEAN(latency_period, (latency_period/10.0)), LOG_NORMAL_FIND_UNDERLYING_SIGMA(latency_period, (latency_period/10.0)));
        rnorm_prodromal_period=lognormal_distribution<float>(LOG_NORMAL_FIND_UNDERLYING_MEAN(prodromal_period, (prodromal_period/10.0)), LOG_NORMAL_FIND_UNDERLYING_SIGMA(prodromal_period, (prodromal_period/10.0)));
        rnorm_test_delay=normal_distribution<float>(test_delay_d, test_delay_d/10.0);
        rnorm_infectious_period=lognormal_distribution<float>(LOG_NORMAL_FIND_UNDERLYING_MEAN(infectious_period, (infectious_period/10.0)), LOG_NORMAL_FIND_UNDERLYING_SIGMA(infectious_period, (infectious_period/10.0)));
        rnorm_trace_delay_manual=normal_distribution<float>(trace_delay_manual_d, trace_delay_manual_d/10.0);
        rnorm_trace_delay_app=normal_distribution<float>(trace_delay_app_d, trace_delay_app_d/10.0);
        sample_I_class=discrete_distribution<int>({p_asymptomatic, p_paucisymptomatic, p_mildsymptomatic, p_severesymptomatic});
    }
}
int main(int argc, char const *argv[])
{
    if(argc>1)
    {
        if(argc>1)
        {
            takeInputFromFile(argv[1]);
        }
    }
    // PRINT PARAMETERS:
    if(false)
    {
        printf("N_students: %d\n"
                "p_app_d: %f\n"
                "p_tested: %f\n"
                "p_test_high_contact: %f\n"
                "p_test_low_contact: %f\n"
                "p_traced: %f\n"
                "p_mask: %f\n"
                "test_delay_d: %f\n"
                "trace_delay_manual_d: %f\n"
                "trace_delay_app_d: %f\n"
                "manual_tracing_threshold: %d\n"
                "app_tracing_threshold: %d\n"
                "mask_reduction_out_d: %f\n"
                "mask_reduction_in_d: %f\n"
                "tracelength_d: %d\n"
                "quarantine_length: %d\n"
                "incubation_period: %f\n"
                "prodromal_period: %f\n"
                "p_asymptomatic: %f\n"
                "p_paucisymptomatic: %f\n"
                "p_mildsymptomatic: %f\n"
                "p_severesymptomatic: %f\n"
                "infectious_period: %f\n"
                "p_transmission: %f\n"
                "low_risk_adjustment: %f\n",
                N_students, p_app_d, p_tested, p_test_high_contact, p_test_low_contact, p_traced, p_mask, test_delay_d, trace_delay_manual_d, trace_delay_app_d, manual_tracing_threshold, app_tracing_threshold, mask_reduction_out_d, mask_reduction_in_d, tracelength_d, quarantine_length, incubation_period, prodromal_period, p_asymptomatic, p_paucisymptomatic, p_mildsymptomatic, p_severesymptomatic, infectious_period, p_transmission, low_risk_adjustment);
    }
    srand(time(0));
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    bool printTimeline=false;
    // float p_mask=0.0, p_app=0.0;
    // string filename="contacts_50000_28.csv";
    vector<vector<pair<int, char>>> eventq;
    unordered_set<int> students_with_apps={};
    vector<vector<tuple<int, int, bool>>> contactdict;
    contactdict.reserve(9000);
    unordered_set<int> student_ids;
    eventq.clear();
    student_ids.clear();
    contactdict.clear();
    string household_edges_file_prev="", outer_edges_file_prev="";
    vector<pair<int, int>> household_edges={}, outer_edges={};

    // int N_students=5000; // NOT A LOCAL VARIABLE, SO DON'T NEED THIS
    vector<nodeClass> studentlist;
    for(int i=0; i<=N_students; i++)
    {
        studentlist.push_back(nodeClass(i));
        if(studentlist[i].has_app)
        {
            students_with_apps.insert(i);
        }
    }
    int curr_time=1;
    int exposed=0, infectious=0, total_infected=0, quarantines=0, false_quaranties=0;
    while(total_infected<6)
    {
        int temp=(rand()%N_students)+1;
        if(studentlist[temp].state=='S')
        {
            studentlist[temp].exposure(eventq, curr_time);
            exposed += 1;
            total_infected += 1;
            if(printTimeline){printf("Patient %d exposed on day %6.2f fro external source.\n", temp, curr_time*timestep_in_data/(float)day);}
        }
    }

    nodeClass::initiatePositiveRecords();
    float temp=0.0;
    int hour;

    vector<tuple<int, int, int, int>> totalCounts{make_tuple(N_students - total_infected, exposed, infectious, total_infected - exposed - infectious)};
    vector<tuple<int, int, char, int, char>> transmissionChain;
    vector<tuple<int, int, char, int, char>> usefulContacts;
    bool done=false;
    pair<int, char> event;
    while(!done)
    {
        if(!eventq[curr_time].empty())
        {
            int l=eventq[curr_time].size();
            for(int i=0; i<l; i++)
            {
                event=eventq[curr_time][i];
                char eventcode=get<1>(event);
                int sid=get<0>(event);
                if(eventcode=='C' && !studentlist[sid].in_quarantine)
                {
                    quarantines+=1;
                    if(printTimeline){printf("Day %6.2f : P%d in quarantine. Q=%d\n", curr_time*timestep_in_data/(float)(day), sid, quarantines);}
                    if(studentlist[sid].state=='S' || studentlist[sid].state=='R')
                    {
                        false_quaranties+=1;
                        if(printTimeline){printf("NOTE: False quarantine. FQ=%d\n", false_quaranties);}
                    }
                }
                else if(eventcode=='R')
                {
                    infectious-=1;
                    if(printTimeline){printf("Day %6.2f : P%d recovered. I=%d\n", curr_time*timestep_in_data/(float)(day), sid, infectious);}
                    // cout<<"At time "<<curr_time<<", E+I="<<(exposed+infectious)<<"\n";
                }
                else if(eventcode=='I')
                {
                    exposed-=1;
                    infectious+=1;
                    if(printTimeline){printf("Day %6.2f : P%d became infectious. E=%d. I=%d\n", curr_time*timestep_in_data/(float)(day), sid, exposed, infectious);}
                }
                else if(eventcode=='J' || eventcode=='K' || eventcode=='L' || eventcode=='M')
                {
                    if(printTimeline){printf("Day %6.2f : P%d in state %c. E=%d. I=%d\n", curr_time*timestep_in_data/(float)(day), sid, eventcode, exposed, infectious);}
                }
                // else if(eventcode=='G')
                // {
                //     if(printTimeline){printf("Day %6.2f : P%d is tested due to tracing.\n", curr_time*timestep_in_data/(float)(day), sid);}
                // }
                studentlist[sid].statechange(eventq, eventcode, curr_time, students_with_apps);
            }
            eventq[curr_time].clear();
        }
        hour = curr_time%(day/timestep_in_data);
        if(hour==1)
        {
            int date = curr_time*timestep_in_data/day;
            if(date<=10)
            {
                generateContactEvents("input_graphs/familyedgelist.csv", "input_graphs/outeredgelist.csv", contactdict, household_edges, outer_edges, household_edges_file_prev, outer_edges_file_prev);
            }
            else
            {
                generateContactEvents("input_graphs/familyedgelist.csv", "input_graphs/outeredgelist.csv", contactdict, household_edges, outer_edges, household_edges_file_prev, outer_edges_file_prev);
            }
        }
        if(!contactdict[hour].empty())
        {
            int l=contactdict[hour].size();
            for(int i=0; i<l; i++)
            {
                int source=get<0>(contactdict[hour][i]), target=get<1>(contactdict[hour][i]);
                bool high_risk = get<2>(contactdict[hour][i]);
                if(!(studentlist[source].state=='S' && studentlist[target].state=='S') && !(studentlist[source].in_quarantine || studentlist[target].in_quarantine) && !(studentlist[source].state=='R' || studentlist[target].state=='R'))
                {
                    studentlist[source].add_contact(target, curr_time, high_risk);
                    studentlist[target].add_contact(source, curr_time, high_risk);
                    bool exposure_possible=false;
                    if(studentlist[source].infectious && studentlist[target].state=='S')
                    {
                        exposure_possible=true;
                    }
                    else if(studentlist[source].state=='S' && studentlist[target].infectious)
                    {
                        source=source^target;
                        target=source^target;
                        source=source^target;
                        exposure_possible=true;
                    }
                    if(exposure_possible)
                    {
                        float damping_lowrisk;
                        if(high_risk)
                        {
                            damping_lowrisk=1.0;
                        }
                        else
                        {
                            damping_lowrisk=low_risk_adjustment;
                        }
                        if(static_cast <float>(rand())/static_cast <float>(RAND_MAX) < p_transmission*damping_lowrisk*studentlist[source].dampingfactor*studentlist[source].mask_factor_out*studentlist[target].mask_factor_in)
                        {
                            studentlist[target].exposure(eventq, curr_time);
                            exposed+=1;
                            total_infected+=1;
                            transmissionChain.push_back(make_tuple(curr_time, source, studentlist[source].state, target, studentlist[target].state));
                            if(printTimeline){printf("Day %6.2f : P%d was exposed to infected P%d. E=%d. total_infected=%d\n", curr_time*timestep_in_data/(float)(day), target, source, exposed, total_infected);}
                        }
                    }
                    usefulContacts.push_back(make_tuple(curr_time, source, studentlist[source].state, target, studentlist[target].state));
                }
            }
        }
        if(exposed+infectious==0)
        {
            done=true;
            // break;
        }
        if((curr_time*timestep_in_data)%day == 0 || done)
        {
            updateTotalCount(curr_time, exposed, infectious, total_infected, totalCounts);
        }
        curr_time+=1;
    }
    cout<<"Done "<<total_infected<<" "<<quarantines<<" "<<false_quaranties<<"\n";
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    // printTotalCount(totalCounts);
    dumpCSVtotalCounts("outputs/dailydata.txt", totalCounts);
    dumpJSONtransmissionChain("outputs/graphdata.json", transmissionChain);
    dumpUsefulContacts("outputs/usefulcontacts.txt", usefulContacts);
    nodeClass::dumpOfficialTestsData("outputs/tests.txt");
    nodeClass::dumpOfficialTracingData("outputs/trace.txt");
    nodeClass::dumpSymptoms("outputs/infectiontypes.txt");
    nodeClass::dumpQuarantineRecords("outputs/quarantines.txt");
    return 0;
}
