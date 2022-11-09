# Sorted Sparse Matrix

Attempt to make an efficient Sparse matrix where insertions and deletions are rare but lookups are common.

## Project allows to check:
1. If two nodes are in relation
2. How many relations there are for node specified
3. Get all relations of node specified

Matrix does not allow duplicates!

## How to use:
Assumption is that nodes are just numbers as in the standard sparse matrix.  
First argument of constructor is strictly necessary to set the number of nodes. 
The other one is purely for c++ vector optimisations.  
Note: There will never be more relations in the matrix than n<sup>2</sup> where n is the number of nodes.  
You can import relations from file if they follow format: "{owner} {relation}\n"

Example code:
```c++
SortedSparseMatrix matrix{1'000, 10'000};
matrix.import_relations_from_file("relations.txt");
// check if in relation
bool in_relation = matrix.is_in_relation(0, 10);
// get number of relations
auto n_relations = matrix.get_number_of_relations(0); // default type is uint32_t
// get all neighbours
auto neighbours = matrix.get_all_relations(0);
for (auto n: neighbours){
    std::cout << n << '\n'; // prints all relations
}
```

### Template arguments clarification:
Relation is a piece of data that holds "with-whom" information. 
Default is uint16_t but if you have more than 65500 nodes then use greater one.  
RelationId_type is a type that is specified for internal use of indexing the relations.
Default is uint32_t but use smaller one if needed

## Custom relations:
SortedSparseMatrix class is a template class which means that you can specify your own type of relation.  
Default relationship type is "who - with whom", so uint32_t type is used.  
However, you can create a new relationship class.
### Requirements of the new class:
1. operator==
2. operator!=
3. operator<
4. operator<<

It should also have some data in it but that is obvious

Example class:

```c++
struct Relation {
    uint16_t with_whom=0;
    float strength_of_relation=1.0;

    bool operator==(const Relation& other) const {
        return this->with_whom == other.with_whom;
    }
    bool operator!=(const Relation& other) const {
        return this->with_whom != other.with_whom;
    }
    bool operator<(const Relation& other) const {
        return this->with_whom < other.with_whom;
    }
    
    friend std::ostream& operator<<(std::ostream& stream, const Relation& obj) const {
        stream << "With whom: " << with_whom << " | Strength of relation: " << strength_of_relation << "\n";
        return stream;
    }
};
```
    

