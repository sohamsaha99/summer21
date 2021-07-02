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

gen.dat.fixed = function(n, p)
{
    C = ceiling(exp(rnorm(n, 3.0, 0.5)))
    I_1 = rbinom(length(C), C, p)
    return(dat.ungroup(I_1, C))
}
gen.dat.fixed.C = function(n, size, p)
{
    C = rep(size, n)
    I = rbinom(length(C), C, p)
    return(dat.ungroup(I, C))
}

