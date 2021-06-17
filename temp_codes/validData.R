tests = read.table("tests_20000_new.txt", sep=",", header=TRUE)
trace = read.table("trace_20000_new.txt", sep=",", header=TRUE)
testsduetotrace = tests[tests$reason==" TRACING" , ]

for(i in 1:nrow(testsduetotrace))
{
	if(length(which(testsduetotrace[i, "test.id"]==trace$test.id))==0)
	{
		print(i)
		break
	}
}
print(i)
testsduetotrace[i, ]

