import warnings
from typing import List, Set
import numpy as np

class Node:
    """
    Class for a node in the network
    """
    def __init__(self, given_id, raw_SAR: float=0.0, sample_collection_time: float=0.0, index: int=-1):
        """
        Initialize the object with given id and index in the vector.
        descendants: Set of indices (not id) which are descendants in the current tree
        contacts: Set of indices denoting contacts
        source: Index (not id) of source
        secondary_attack_rate: Self explanatory. Should provide some value computed from the data
        """
        self.id = given_id
        self.index = index
        self.descendants = set()
        self.contacts = set()
        self.source = None
        self.secondary_attack_rate = raw_SAR
        self.sample_collection_time = sample_collection_time
    
    def add_contacts(self, set_of_contacts: Set):
        """
        Insert the set of contacts as contacts of the node
        """
        self.contacts.update(set_of_contacts)
    
    def insert_source(self, source: int):
        """
        Insert an index as source of the node
        """
        if self.source is not None:
            warnings.warn("Warning for node id: {}, index: {}. Source was already present. Previous Source: {}. New Source: {}".format(self.id, self.index, self.source, source))
        self.source = source
    
    def update_descendants(self, new_descendants: Set):
        """
        Update the descendants set by inserting new elements
        """
        self.descendants.update(new_descendants)
    
    def print_node_info(self):
        """
        Prints all info associated with the node
        """
        print("Printing information of Node id: {}, index: {}.\n Indices of descendants: {}\n Indices of contacts: {}\n Source index: {}, SAR = {}, Sample collection time: {}".format(self.id, self.index, self.descendants, self.contacts, self.source, self.secondary_attack_rate, self.sample_collection_time))

def criteria_function_1(x: List[float], y: List[float]):
    """
    Returns an index chosen randomly by treating x as a vector of weights
    """
    if len(x) == 0 or len(y) == 0:
        return -1
    s = np.sum(x)
    return np.random.choice(len(x), p = [(i/s) for i in x])

def find_source_of_node(index: int, vector_of_nodes: List[Node], criteria_function):
    """
    Find the source of the node given by vector_of_nodes[index],
    where vector_of_nodes is an array with elements of class Node.
    """
    case_sample_collection_time = vector_of_nodes[index].sample_collection_time
    contacts_of_case = vector_of_nodes[index].contacts
    descendants_of_case = vector_of_nodes[index].descendants
    candidates_for_source = list(contacts_of_case.difference(descendants_of_case))
    SAR_values = [vector_of_nodes[i].secondary_attack_rate for i in candidates_for_source]
    time_differences = [(vector_of_nodes[i].sample_collection_time - case_sample_collection_time) for i in candidates_for_source]
    source_index = criteria_function(SAR_values, time_differences)
    # source_index == -1 iff candidates_for_source == []
    if source_index != -1:
        estimated_source = candidates_for_source[source_index]
        vector_of_nodes[index].insert_source(estimated_source)
        update_descendants_upward(index, vector_of_nodes)

def update_descendants_upward(index: int, vector_of_nodes: List[Node]):
    """
    Updates the descendants sets of vector_of_nodes[index].source
    And continues doing it recursively by going upward along the tree
    """
    extra_descendants = vector_of_nodes[index].descendants
    extra_descendants.add(index) # Add the node itself
    node_index = vector_of_nodes[index].source
    while node_index is not None:
        vector_of_nodes[node_index].update_descendants(extra_descendants)
        node_index = vector_of_nodes[node_index].source
