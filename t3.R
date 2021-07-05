library(dplyr)
tests = read.table("tests.txt", sep=",", header=TRUE)
# tests = tests[tests$collect>=200, ]
trace = read.table("trace.txt", sep=",", header=TRUE)

tests.p <- tests[tests$result==" POSITIVE",]
pos <- match(trace$contact.id,tests.p$id)
diff <- abs(trace$tracing.time-tests.p$result.date[pos])
pos.new <- which(diff<=4)
# pos.new gives the indices for the useful traces
trace.use <- trace[pos.new,]

case.date <- tests.p$result.date[match(trace.use$case.id,tests.p$id)]
contact.date <- tests.p$result.date[match(trace.use$contact.id,tests.p$id)]
t3 <- contact.date-case.date

transmission = read.table("transmissions.csv", sep=',', header=TRUE)

trueSources.contacts = transmission$source[match(trace.use$contact.id, transmission$target)]
trueSources.cases = transmission$source[match(trace.use$case.id, transmission$target)]
b1 = (trueSources.contacts==trace.use$case.id) # Contains TRUE in a position if case infected contact
sum(b1, na.rm=TRUE) # Few NAs might happen as the first 6 exposed dont have any source
b2 = (trueSources.cases==trace.use$contact.id) # Contains TRUE in a position if contact infected case
sum(b2, na.rm=TRUE)
sum(b1 & b2, na.rm=TRUE) # Should be 0 if everything is correct

df = data.frame(t3=t3, b1=b1, b2=b2)
df = df %>% mutate(category=ifelse(b1, "FORWARD", ifelse(b2, "BACKWARD", "NOTHING")))
boxplot(t3~category, df)

