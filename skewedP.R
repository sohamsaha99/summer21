library(lme4)

n = 100
set.seed(1)
par(mfrow=c(2, 3))
# Skewness in degree
C = ceiling(exp(rnorm(n, 3.0, 0.7)))
hist(C)

# p is constant
p = 0.2
I_1 = rbinom(length(C), C, p)
hist(I_1)
hist(I_1/C)

#Skewness p_transmission
mu = log(p) - log(1-p) # So mode is same as p defined above
sigma = 1.0 # Larger sigma => Higher skewnwss
x = ppoints(100)
plot(x, dnorm(log(x/(1-x)), mu, sigma)/(x*(1-x)), ty='l', xlab='p', ylab='density', main=paste0("logit()~N(", round(mu, 2), ", ", sigma, "^2)"))
abline(v=exp(qnorm(0.95, mu, sigma))/(1+exp(qnorm(0.95, mu, sigma))), lty=2)
normals = rnorm(n, mu, sigma)
p_trans = exp(normals)/(1+exp(normals))
I_2 = rbinom(length(C), C, p_trans)
hist(I_2)
hist(I_2/C)

# Fitting GLMM
source("utilsOverdispersion.R")
df1 = dat.ungroup(I_1, C)
model1 = glmer(outcome~1|group, family='binomial', data=df1)
print("##### MODEL 1 #####")
print(summary(model1))
# as.data.frame(VarCorr(model1))[1, 5]

df2 = dat.ungroup(I_2, C)
model2 = glmer(outcome~1|group, family='binomial', data=df2)
print("##### MODEL 2 #####")
print(summary(model2))

B = 1000
vrnc = NULL
for(b in 1:B)
{
    df = gen.dat.fixed(n, p)
    model = glmer(outcome~1|group, family='binomial', data=df)
    vrnc[b] = as.data.frame(VarCorr(model))[1, 4]
    if(b%%100==0){print(b)}
}

