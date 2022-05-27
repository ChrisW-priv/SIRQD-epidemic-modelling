from random import random


choices = list(range(10_000))


def make_relations(text_file):
    relations = []
    for i in range(10_000):
        for j in range(i+1, 10_000):
            in_relation = random() < (1/(10_000/10))
            if in_relation:
                relations.append((i, j))
                relations.append((j, i))

    sorted_relations = sorted(relations)

    with open(text_file, "w") as new:
        for a, b in sorted_relations:
            new.write(f"{a} {b}\n")


make_relations("who_knows_who.txt")
make_relations("who_meets_who.txt")
