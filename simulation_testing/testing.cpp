#define ROUND_2_INT(f) ((int)((f) >= 0.0 ? ((f) + 0.5) : ((f) - 0.5)))

#define LOG_NORMAL_FIND_UNDERLYING_MEAN(mu, sgm) (log((mu) /sqrt(1 + ((sgm) * (sgm) / ((mu) * (mu))))))

#define LOG_NORMAL_FIND_UNDERLYING_SIGMA(mu, sgm) (sqrt(log(1 + ((sgm) * (sgm) / ((mu) * (mu))))))

#include<bits/stdc++.h>
#include<chrono>

using namespace std;

random_device rd;
mt19937 gen(rd());

normal_distribution<float> generateFromNormal(3.0, 2.0);
lognormal_distribution<float> generateFromLognormal(LOG_NORMAL_FIND_UNDERLYING_MEAN(50.0, 10.0), LOG_NORMAL_FIND_UNDERLYING_SIGMA(50.0, 10.0));

// Call this in main() and pipe the output to a file.
// Then check whether the mean variance are correct using R ot Python.
int TEST_NORMAL_GENERATOR()
{
    for(int i=0; i<10000; i++)
        printf("%f\n", generateFromNormal(gen));
    return 0;
}

int TEST_LOGNORMAL_GENERATOR()
{
    printf("X\n");
    for(int i=0; i<10000; i++)
        printf("%f\n", generateFromLognormal(gen));
    return 0;
}

int TEST_VECTOR_INSERT(void)
{
    vector<pair<int, char>> vector_of_num;
    int a = vector_of_num.size();
    if(FILE *fp = fopen("dfkjg.txt", "r"))
    {
        fprintf("%d, %d", a, a);
        fclose(fp);
    }
}

int TEST_LOGNORMAL_GEN(void)
{
    for (int i = 0; i < 10000; i++)
    {
        printf("%f\n", generateFromLognormal(gen));
    }
    return 0;
}

class testingClass
{
private:
    int x, y;
public:
    testingClass(int a, int b);
    ~testingClass();
};

testingClass::testingClass(int a, in bt)
{
    x = a;
    y = b;
}

testingClass::~testingClass()
{
    print
}


int main()
{
    // TEST_NORMAL_GENERATOR();
    TEST_LOGNORMAL_GENERATOR();
    return 0;
}