import csv
import numpy as np
from networkx import expected_degree_graph, selfloop_edges, write_edgelist
n = 200 # Number of nodes. Node id: 1, ..., 500

# h1 = 269898 # 1 person
# h2 = 209573 # 2 person
# h3 = 152959 # 3 person
# h4 = 122195 # 4 person
# h5 = 43327 # 5 person
# h6 = 17398 # 6 person
# h7 = 6073 # 7 person
# h8 = 3195 # 8 person

house_size_probs = [269898, 209573, 152959, 122195, 43327, 17398, 6073, 3195]
house_size_probs = [x/sum(house_size_probs) for x in house_size_probs]
household_sizes = []
household_edges = []
num = 0
while num < n:
    temp = np.random.choice(np.arange(1, len(house_size_probs)+1), 1, p=house_size_probs)[0]
    if num+temp>n:
        temp = n-num
    household_sizes.append(temp)
    if temp>1:
        for i in range(1, temp):
            for j in range(i+1, temp+1):
                household_edges.append([num+i, num+j])
    num += temp

with open("familyedgelist.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerows(household_edges)

outer_degrees = np.random.gamma(shape=1.65 ,scale=0.61, size=n)
G = expected_degree_graph(outer_degrees)
G.remove_edges_from(selfloop_edges(G))

outer_edges = []
for u, v in G.edges:
    outer_edges.append([u+1, v+1])

with open("outeredgelist.csv", "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerows(outer_edges)

# fh = open("outeredgelist.csv", "wb")
# write_edgelist(G, fh, data=False, delimiter=",")
