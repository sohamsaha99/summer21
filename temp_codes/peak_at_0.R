library(dplyr)
library(ggplot2)
infectiontypes = read.table("outputs/infectiontypes.txt", sep=",", header=TRUE, strip.white=TRUE)
quarantines = read.table("outputs/quarantines.txt", sep=",", header=TRUE, strip.white=TRUE)
transmissions = read.table("outputs/transmissions.csv", sep=",", header=TRUE, strip.white=TRUE)
tests = read.table("outputs/tests.txt", sep=",", header=TRUE, strip.white=TRUE)
tests = tests[tests$result=="POSITIVE", ]

setIntersectionLength = function(x1, y1, x2, y2) # Length of intersection of [x1, y1] and [x2, y2]
{
    if(x1>=y1 | x2>=y2)
    {
        return(0)
    }
    if(y1<=x2)
    {
        return(0)
    }
    if(y2<=x1)
    {
        return(0)
    }
    if(x2<=x1 & y2>=y1) # x2, x1, y1, y2
    {
        return(y1 - x1)
    }
    if(x2<=x1 & y2<=y1) # x2, x1, y2, y1
    {
        return(y2 - x1)
    }
    if(x2>=x1 & y2>=y1) # x1, x2, y1, y2
    {
        return(y1 - x2)
    }
    if(x2>=x1 & y2<=y1) # x1, x2, y2, y1
    {
        return(y2 - x2)
    }
}

findUnquarantinePeriod = function(i, begin, end)
{
    quarantines.subset = quarantines %>% filter(id==i)
    if(nrow(quarantines.subset)==0)
    {
        return(end - begin)
    }
    deducted.time=0
    for(i in 1:(nrow(quarantines.subset)/2))
    {
        deducted.time = deducted.time + setIntersectionLength(begin, end, quarantines.subset[2*i-1, "time"], quarantines.subset[2*i, "time"])
    }
    return(end - begin - deducted.time)
}
findUnquarantinePeriodVectorized = Vectorize(findUnquarantinePeriod)

countSecondaryInfections = function(i)
{
    sum(transmissions$source==i)
}
countSecondaryInfectionsVectorized = Vectorize(countSecondaryInfections)

infectiontypes.new = infectiontypes %>% 
                        mutate(effective.infectious.time=findUnquarantinePeriodVectorized(id, presympt.time, removed.time)) %>%
                        mutate(secondary.infections=countSecondaryInfectionsVectorized(id))
infectiontypes.merged_at_10 = infectiontypes.new %>% 
                                mutate(secondary.infections=ifelse(secondary.infections<10, secondary.infections, 10)) %>%
                                mutate(inclusion=(id %in% tests$id))

boxplot(effective.infectious.time ~ secondary.infections , infectiontypes.merged_at_10)
boxplot(effective.infectious.time ~ secondary.infections , infectiontypes.merged_at_10 %>% filter(inclusion))
boxplot(effective.infectious.time ~ secondary.infections , infectiontypes.merged_at_10 %>% filter(!inclusion))

stat_box_data <- function(y, upper_limit = 1.15*max(infectiontypes.merged_at_10$effective.infectious.time)) {
  return( 
    data.frame(
      y = 0.95 * upper_limit,
      label = paste('count =', length(y), '\n',
                    'mean =', round(mean(y), 1), '\n')
    )
  )
}

myboxplot = function(dat)
{
    ggplot(dat, aes(factor(secondary.infections), effective.infectious.time)) + 
                geom_boxplot() + 
                stat_summary(
                    fun.data = stat_box_data, 
                    geom = "text", 
                    hjust = 0.5,
                    vjust = 0.9
                  )
}

myboxplot(infectiontypes.merged_at_10)
myboxplot(infectiontypes.merged_at_10 %>% filter(inclusion))
myboxplot(infectiontypes.merged_at_10 %>% filter(!inclusion))

prop.table(table(infectiontypes.merged_at_10 %>% select(secondary.infections)))
prop.table(table(infectiontypes.merged_at_10 %>% filter(inclusion) %>% select(secondary.infections)))
prop.table(table(infectiontypes.merged_at_10 %>% filter(!inclusion) %>% select(secondary.infections)))
