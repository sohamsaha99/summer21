library(lme4)
n = 100
size = 24
p = 0.2
C = rep(size, n)
I = rbinom(length(C), C, p)
source("utilsOverdispersion.R")
B = 1000
vrnc = NULL
for(b in 1:B)
{
    df = gen.dat.fixed.C(n, size, p)
    model = glmer(outcome~1|group, family='binomial', data=df)
    vrnc[b] = as.data.frame(VarCorr(model))[1, 4]
    if(b%%100==0){print(b)}
}

