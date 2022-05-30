from random import random


choices = 10_000
avg_friends = 10


def make_relations(text_file):
    relations = []
    for i in range(choices):
        for j in range(i+1, choices):
            in_relation = random() < (1/(choices/avg_friends))
            if in_relation:
                relations.append((i, j))
                relations.append((j, i))

    sorted_relations = sorted(relations)

    with open(text_file, "w") as new:
        for a, b in sorted_relations:
            new.write(f"{a} {b}\n")


make_relations("who_knows_who.txt")
make_relations("who_meets_who.txt")
