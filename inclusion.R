tests = read.table("tests.txt", sep=",", header=TRUE)
transmission = read.table("transmissions.csv", sep=',', header=TRUE)
tests.p <- tests[tests$result==" POSITIVE",]
v = NULL
v['0']=0
for(i in 1:100)
{
    v[as.character(i)] = 0
}
tab = table(transmission$source)
for(i in 1:nrow(tests.p))
{
    if(!is.na(tab[as.character(tests.p$id[i])]))
    {
        v[as.character(tab[as.character(tests.p$id[i])])] = v[as.character(tab[as.character(tests.p$id[i])])] + 1
    }
    else
    {
        v['0'] = v['0']+1
    }
}
original = table(table(transmission$source))
original['0'] = 32457 - sum(original) #####################################################################################

