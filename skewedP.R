library(lme4)

n = 200
set.seed(2001)
par(mfrow=c(2, 3))
# Skewness in degree
C = round(exp(rnorm(n, 3, 0.3)))
hist(C)

# p is constant
p = 0.1
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
dat.ungroup = function(I, C)
{
    k = 0
    n = length(C)
    df = list(outcome=NA, group=NA)
    for(i in 1:n)
    {
        df$outcome[k + 1:I[i]] = 1
        df$group[k + 1:I[i]] = paste(i)
        df$outcome[k + I[i] + 1:(C[i]-I[i])] = 0
        df$group[k + I[i] + 1:(C[i]-I[i])] = paste(i)
        k = k + C[i]
    }
    return(data.frame(df))
}
df1 = dat.ungroup(I_1, C)
model1 = glmer(outcome~1|group, family='binomial', data=df1)
print("##### MODEL 1 #####")
print(summary(model1))
df2 = dat.ungroup(I_2, C)
model2 = glmer(outcome~1|group, family='binomial', data=df2)
print("##### MODEL 2 #####")
print(summary(model2))
# model0 = glmer(cbind(I_2, C-I_2)~1|1, family='binomial')
