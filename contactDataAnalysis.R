library(igraph)
day = 24*60*60
max_time = 28*day
N_population = 500
n_known_per_person = 50
n_contacts_per_day_per_person = 12
p_edge = n_known_per_person/(N_population-1)
contact_rate_per_sec = N_population*n_contacts_per_day_per_person/(2*day)
g = erdos.renyi.game(N_population, p_edge, "gnp")
degree_distribution(g)
plot(g, vertex.size=1, vertex.label=NA)
edges = as_edgelist(g)
sources = edges[, 1]
targets = edges[, 2]

expected_arrivals_per_day = round(day*contact_rate_per_sec)
arrival_times = cumsum(rexp(expected_arrivals_per_day, rate = contact_rate_per_sec))
while(arrival_times[length(arrival_times)] < 28*day)
{
    arrival_times[1:expected_arrivals_per_day + length(arrival_times)] = cumsum(rexp(expected_arrivals_per_day, rate = contact_rate_per_sec))
}
