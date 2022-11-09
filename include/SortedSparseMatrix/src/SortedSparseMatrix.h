#ifndef SORTEDSPARSEMATRIX_H
#define SORTEDSPARSEMATRIX_H

#include <vector>
#include <span>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <fstream>

/* Optimised version of Sparse matrix
 * Focused on quick neighbouring nodes access
 * We split relation into owner and the rest of the relation
*/


template<typename Relation=uint16_t, typename RelationId_type=uint32_t>
class SortedSparseMatrix {
    /// vector indicating the position where relations of an owner start
    std::vector<RelationId_type> starting_positions;
    /// vector of all relations in the matrix
    std::vector<Relation> relations;

    /// updates the position of boundaries for all owners after index
    void update_boundaries(RelationId_type start_index, RelationId_type by_how_much){
        for (auto i = start_index+1; i < starting_positions.size(); ++i) {
            starting_positions[i] += by_how_much;
        }
    }

    /// finds where relations of an owner start
    [[nodiscard]] RelationId_type find_position_of_relation(RelationId_type owner, Relation relation) const{
        return binary_search_in_relations(relation, find_start_of_relations(owner), find_start_of_relations(owner+1));
    }

    /// binary search: looks for element between low and high indexes. If not found returns where element should be
    [[nodiscard]] RelationId_type binary_search_in_relations(Relation relation, RelationId_type low, RelationId_type high) const{
        high++;
        RelationId_type mid=low;
        while (low != high) {
            mid = (low + high) / 2;

            if (relations[mid] == relation)
                return mid;
            if (relations[mid] < relation)
                low = mid+1;
            else
                high = mid-1;
        }

        return mid;
    }

    /// finds where relations of owner begin
    [[nodiscard]] RelationId_type find_start_of_relations(RelationId_type owner) const {
        return starting_positions[owner];
    }

    /// returns copy of relation at index
    [[nodiscard]] const Relation& get_relation_at_index(RelationId_type index) const {
        return relations[index];
    }
public:
    /// default constructor. Number of nodes is essential to be set to correct number!!!
    /// If too little nodes code does not resize the private vector!!!
    explicit SortedSparseMatrix(size_t N_nodes=10, size_t N_relations=100){
        starting_positions.resize(N_nodes+1);
        relations.resize(N_relations);
    }

    /// imports relations from file specified. Format of file should be "{owner} {relation}" separated by new line
    void import_relations_from_file(const char * filename){
        std::ifstream file{filename};
        if (!file) std::cout << "couldn't open file\n";

        Relation relation;
        RelationId_type owner;
        while (file){
            file >> owner;
            file >> relation;
            this->insert_relation(owner, Relation{relation});
        }
    }

    /// imports relations from file specified. Format of file should be "{owner} {relation}" separated by new line. \n
    /// relations have to be sorted by owner and relation!!
    void import_sorted_relations_from_file(const char * filename){
        std::ifstream file{filename};
        if (!file) std::cout << "couldn't open file\n";

        Relation relation;
        RelationId_type owner, last_owner;

        // import first relation
        file >> last_owner;
        file >> relation;
        this->insert_relation(last_owner, Relation{relation});
        RelationId_type row = 1, last_row = 1;

        while (file){
            file >> owner;
            file >> relation;
            if (owner != last_owner){
                update_boundaries(last_owner, row-last_row);
                last_owner = owner;
                last_row = row;
            }
            relations.insert(relations.begin() + row, relation);
            ++row;
        }
        update_boundaries(last_owner, row-last_row);
    }

    /// inserts relation and updates the boundaries vector
    void insert_relation(RelationId_type owner, Relation relation){
        // look for index
        auto found_element = find_position_of_relation(owner, relation);
        // if already in relation we don't want to add another relation
        if (get_relation_at_index(found_element) == relation && get_number_of_relations(owner) != 0) throw std::invalid_argument("nodes already in relation");

        // if all good, insert into position
        auto position_to_insert = relations.begin() + found_element;
        relations.insert(position_to_insert, relation);
        update_boundaries(owner, 1);
    }

    /// removes relation and updates the boundaries vector
    void remove_relation(RelationId_type owner, Relation relation){
        auto element_found = find_position_of_relation(owner, relation);
        if (get_relation_at_index(element_found) != relation) throw std::invalid_argument("relation found doesn't matched specified");

        relations.erase(relations.begin() + element_found);
        update_boundaries(owner, -1);
    }

    /// checks if owner is in possession of relation
    [[nodiscard]] bool is_in_relation(RelationId_type owner, Relation relation) const {
        auto found_element = find_position_of_relation(owner, relation);
        return get_relation_at_index(found_element) == relation && get_number_of_relations(owner) != 0;
    }

    /// returns number of relations belonging to owner
    [[nodiscard]] RelationId_type get_number_of_relations(RelationId_type owner) const {
        return find_start_of_relations(owner+1) - find_start_of_relations(owner);
    }

    /// returns all relations belonging to owner.
    [[nodiscard]] std::span<const Relation> get_all_relations(RelationId_type owner) const {
        auto start_ = relations.begin() + find_start_of_relations(owner);
        auto end_ = relations.begin() + find_start_of_relations(owner+1);
        return {start_, end_};
    }

    [[nodiscard]] RelationId_type get_number_of_nodes() const {
        return starting_positions.size()-1;

    }

    /// standard output of SortedSparseMatrix.
    /// Returns stream with information on number of nodes + lists each relation owner has
    friend std::ostream& operator<<(std::ostream& stream, const SortedSparseMatrix& obj){
        stream<<"Number of Nodes: " << obj.get_number_of_nodes() << '\n';
        Relation owner = 0;
        Relation index = 0;
        while (owner < obj.starting_positions.size()-1){
            std::cout << "Neighbours of node: " << owner << ":\n";
            auto N_relations = obj.get_number_of_relations(owner);
            for (int i = 0; i < N_relations; ++i) {
                std::cout << obj.get_relation_at_index(index) << '\n';
                ++index;
            }
            ++owner;
        }
        return stream;
    }
};

#endif //SORTEDSPARSEMATRIX_H
