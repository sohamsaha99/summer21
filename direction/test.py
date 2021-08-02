# %%
from assign_direction import *
# %%
import matplotlib.pylab as plt
from typing import List, Set

# %%
plt.hist([criteria_function_1(range(50), range(2, 52)) for _ in range(10000)])
# %%
node = Node('ABC', 0.2, 5.2, 0)
node.print_node_info()
# %%
node.add_contacts(set([1, 5, 9, 11]))
node.print_node_info()
# %%
node.insert_source(11)
node.insert_source(9)
node.print_node_info()
# %%
node.add_contacts(set([11, 15, 17]))
node.update_descendants(set([14, 16, 18]))
node.update_descendants(set([16, 20]))
node.print_node_info()
# %%
## Testing of update_descendants_upward
vector_of_nodes: List[Node] = []

# Node 0
node = Node("P0", 0.0, 0.0, 0)
node.update_descendants(set([1, 2, 3]))
node.insert_source(11)
vector_of_nodes.append(node)

#Node 1
node = Node("P1", index=1)
node.update_descendants(set([]))
node.insert_source(0)
vector_of_nodes.append(node)

#Node 2
node = Node("P2", index=2)
node.update_descendants(set([4, 5]))
node.insert_source(0)
vector_of_nodes.append(node)

#Node 3
node = Node("P3", index=3)
node.update_descendants(set([6]))
node.insert_source(0)
vector_of_nodes.append(node)

#Node 4
node = Node("P4", index=4)
node.insert_source(2)
vector_of_nodes.append(node)

#Node 5
node = Node("P5", index=5)
node.insert_source(2)
vector_of_nodes.append(node)

#Node 6
node = Node("P6", index=6)
node.insert_source(3)
vector_of_nodes.append(node)

#Node 7
node = Node("P7", index=7)
node.update_descendants(set([8 ,9]))
vector_of_nodes.append(node)

#Node 8
node = Node("P8", index=8)
node.update_descendants(set([10]))
node.insert_source(7)
vector_of_nodes.append(node)

#Node 9
node = Node("P9", index=9)
node.insert_source(7)
vector_of_nodes.append(node)

#Node 10
node = Node("P10", index=10)
node.insert_source(8)
vector_of_nodes.append(node)

#Node 11
node = Node("P11", index=11)
node.update_descendants(set([0]))
vector_of_nodes.append(node)

# %%
## Test continued
## Add a source but dont update descendants
vector_of_nodes[7].insert_source(3)
update_descendants_upward(7, vector_of_nodes)
# %%
for node in vector_of_nodes:
    node.print_node_info()
    print("-----------------------------\n")
# %%
