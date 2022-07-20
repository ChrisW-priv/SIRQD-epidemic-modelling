import networkx as nx


def make_relations(text_file: str, m_defining_network: int, n_nodes: int, seed=0):
    graph = nx.barabasi_albert_graph(n_nodes, m_defining_network, seed=seed)

    ''' 
    HERE YOU CAN ADD THE GROUPING PART 
    IF YOU DO IT LATER YOU MIGHT FIND THAT THERE WILL BE PROBLEMS WITH MISMATCHING DATA!!!
    '''

    edges = list(graph.edges)
    relations = []

    # add all relations given by .edges property
    relations += edges

    # also add reversed edges - if a knows b, b knows a
    for edge in edges:
        a, b = edge
        relations.append((b, a))

    sorted_relations = sorted(relations)

    with open(text_file, "w") as new:
        for a, b in sorted_relations:
            new.write(f"{a} {b}\n")


def create_random_networks_with_different_seeds(name: str,
                                                m_defining_network: int,
                                                n_nodes: int,
                                                number_of_networks: int = 5):

    """creates a number of networks all different from another based on different seed values"""

    for n in range(number_of_networks):
        filename = f"../data/{name}{n+1}.txt"
        print('creating', filename)
        make_relations(filename, m_defining_network, n_nodes, seed=n)


if __name__ == '__main__':
    m_epidemic = 10
    m_opinion = 10
    n_nodes = 10_000
    n_networks = 5

    create_random_networks_with_different_seeds("who_knows_who", m_opinion, n_nodes, n_networks)
    create_random_networks_with_different_seeds("who_meets_who", m_epidemic, n_nodes, n_networks)
