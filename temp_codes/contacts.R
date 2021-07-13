library(dplyr)
infectiontypes = read.table("outputs/infectiontypes.txt", sep=",", header=TRUE, strip.white=TRUE)
usefulcontacts = read.table("outputs/usefulcontacts.txt", sep=",", header=TRUE, strip.white=TRUE)
tests = read.table("outputs/tests.txt", sep=",", header=TRUE, strip.white=TRUE)
tests = tests[tests$result=="POSITIVE", ]

infectionStatus = function(id, infectionTypes=infectiontypes)
{
    return(ifelse(length(ret <- infectiontypes[infectiontypes$id==id, "symptom.type"])==0, "SUSCEPTIBLE", ret))
}

countFirstDegreeContacts = function(i, usefulContacts=usefulcontacts, infectionTypes=infectiontypes)
{
    # infectionTypes = infectionTypes[infectionTypes$symptom != "ASYMPTOMATIC", ]
    usefulContacts = usefulContacts[usefulContacts$P1==i | usefulContacts$P2==i, ]
    usefulContacts = usefulContacts %>% mutate(id=ifelse(P1==i, P2, P1))
    usefulContacts = usefulContacts %>% inner_join(infectionTypes, by="id")
    usefulContacts = usefulContacts %>% mutate(state=ifelse(P1==i, S2, S1))
    usefulContacts = usefulContacts[usefulContacts$state %in% c("K", "L", "M") | abs(usefulContacts$time.x - usefulContacts$time.y)<=7, ]
    return(length(unique(usefulContacts$id)))
    # return(usefulContacts)
}

checkIfFirstDegree = function(p1, p2, usefulContacts=usefulcontacts, infectionTypes=infectiontypes)
{
    usefulContacts = usefulContacts[(usefulContacts$P1==p1 & usefulContacts$P2==p2) | (usefulContacts$P1==p2 & usefulContacts$P2==p1), ]
    usefulContacts = usefulContacts %>% mutate(state=ifelse(P1==p1, S2, S1))
    times = usefulContacts$time
    time.i = infectionTypes[infectiontypes$id==p2, "time"]
    # print(usefulContacts)
    # print(time.i)
    # print(length(times))
    if((length(times) > 0 && min(abs(times-time.i)) <= 7) | sum(c("K", "L", "M") %in% usefulContacts$state) > 0)
    {
        return(TRUE)
    }
    else
    {
        return(FALSE)
    }
}

countSecondDegreeContacts = function(id, usefulContacts=usefulcontacts, infectionTypes=infectiontypes)
{
    k = 0
    secondDegreeContacts = NULL
    for(i in 1:nrow(infectionTypes))
    {
        P2 = infectionTypes[i, "id"]
        if(id != P2 && checkIfFirstDegree(id, P2, usefulContacts, infectionTypes))
        {
            for(j in 1:nrow(infectionTypes))
            {
                P3 = infectionTypes[j, "id"]
                if(P3 != id && P3 != P2 && checkIfFirstDegree(P2, P3) && !checkIfFirstDegree(id, P3))
                {
                    k = k + 1
                    secondDegreeContacts[k] = P3
                }
            }
        }
    }
    # return(secondDegreeContacts)
    return(k)
}

getCountsForAll = function(infectionTypes=infectiontypes, usefulContacts=usefulcontacts, tests.pos=tests)
{
    firstDegreeCounts = NULL
    included = NULL
    for(i in 1:nrow(infectionTypes))
    {
        id = infectionTypes[i, "id"]
        if(infectionStatus(id)!="SUSCEPTIBLE")
        {
            firstDegreeCounts[i] = countFirstDegreeContacts(id)
        }
        else
        {
            firstDegreeCounts[i] = 0
        }
        if(id %in% tests.pos$id)
        {
            included[i] = TRUE
        }
        else
        {
            included[i] = FALSE
        }
    }
    return(cbind(infectionTypes, firstDegreeCounts, included))
}

if(FALSE)
{
    df = getCountsForAll()
    tab=table(df$firstDegreeCounts, df$included)
}