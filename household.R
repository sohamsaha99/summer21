house_size_probs = c(269898, 209573, 152959, 122195, 43327, 17398, 6073, 3195)
house_size_probs = house_size_probs / sum(house_size_probs)

n = 5000 # Number of households. Id: 1, ..., 5000
num = 0
household_sizes = c()
household_edges = list()
k = 0
l = 0
while(num < n)
{
    temp = sample(length(house_size_probs), 1, prob=house_size_probs)
    if(num + temp > n)
    {
        temp = n-num
    }
    l = l + 1
    household_sizes[l] = temp
    if(temp > 1)
    {
        for(i in 1:(temp-1))
        {
            for(j in (i+1):temp)
            {
                k = k + 1
                household_edges[[k]] = c(i, j) + num
            }
        }
    }
    num = num + temp
}

outer_degrees = rgamma(n, shape=1.65, scale=0.61)
