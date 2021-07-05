#include <bits/stdc++.h>
using namespace std;

const int day=24*60*60;
const int timestep_in_data=300;

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
void generateContactEvents(string household_edges_file, string outer_edges_file, vector<vector<tuple<int, int, bool>>> &contactdict, vector<pair<int, int>> &household_edges, vector<pair<int, int>> &outer_edges, string &household_edges_file_prev, string &outer_edges_file_prev)
{
    if(household_edges_file_prev!=household_edges_file)
    {
        printf("Got new file for family.\n");
        // Read household_edges_file and insert in household_edges
        int P1, P2, k=0;
        if(FILE *fp=fopen(household_edges_file.c_str(), "r"))
        {
            household_edges.clear();
            while(fscanf(fp, "%d,%d",&P1, &P2)==2)
            {
                k++;
                household_edges.push_back({P1, P2});
            }
            fclose(fp);
            household_edges_file_prev=household_edges_file;
        }
    }
    if(outer_edges_file_prev!=outer_edges_file)
    {
        printf("Got new file for outer.\n");
        // Read outer_edges_file and insert in outer_edges
        int P1, P2, k=0;
        if(FILE *fp=fopen(outer_edges_file.c_str(), "r"))
        {
            outer_edges.clear();
            while(fscanf(fp, "%d,%d",&P1, &P2)==2)
            {
                k++;
                outer_edges.push_back({P1, P2});
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

int main(int argc, char const *argv[])
{
    srand(time(0));
    string household_edges_file_prev, outer_edges_file_prev;
    vector<vector<tuple<int, int, bool>>> contactdict;
    vector<pair<int, int>> household_edges, outer_edges;
    generateContactEvents("familyedgelist.csv", "outeredgelist.csv", contactdict, household_edges, outer_edges, household_edges_file_prev, outer_edges_file_prev);
    for(int i=0; i<contactdict.size(); i++)
    {
        for(int j=0; j<contactdict[i].size(); j++)
        {
            cout<<"At time "<<i<<", contact between P"<<get<0>(contactdict[i][j])<<" and P"<<get<1>(contactdict[i][j])<<". Type="<<get<2>(contactdict[i][j])<<"\n";
        }
    }
    return 0;
}