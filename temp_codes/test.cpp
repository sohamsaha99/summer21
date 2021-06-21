#include <bits/stdc++.h>
using namespace std;
float b=10.0;
int c=-1;
int main(int argc, char const *argv[])
{
    if(argc>1)
    {
        if(FILE *fp=fopen(argv[1], "r"))
        {
            fscanf(fp, "%f", &b);
            printf("%f\n", b);
            fscanf(fp, "%d\n", &c);
            printf("%d\n", c);
        }
    }
    // cout<<b;
    return 0;
}