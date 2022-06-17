import networkx as nx

n_nodes = 10_000


def make_relations(text_file, m):
    graph = nx.barabasi_albert_graph(n_nodes, m)

    edges = [edge for edge in graph.edges]
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


m_epidemic = 10
m_opinion = 10

make_relations("../data/who_knows_who.txt", m_opinion)
make_relations("../data/who_meets_who.txt", m_epidemic)
