#include <iostream>
#include "SortedSparseMatrix.h"
#include "SIRQD_bulk.h"


int main() {
    // init parameters of the simulation
    constexpr uint16_t n_agents = 10'000;
    constexpr uint8_t n_steps = 25;
    constexpr uint16_t n_infected_agents = 500;
    constexpr uint16_t n_negative_agents = 500;
    constexpr uint8_t q_size_of_lobby = 10;

    // init adjacency matrix
    SortedSparseMatrix who_knows_who{n_agents, 20 * n_agents};
    SortedSparseMatrix who_meets_who{n_agents, 20 * n_agents};

    // import relations between agents from file
    std::cout << "importing relations from files...\n";
    who_knows_who.import_sorted_relations_from_file("../data/who_knows_who1.txt");
    who_meets_who.import_sorted_relations_from_file("../data/who_meets_who1.txt");
    std::cout << "done importing relations\n";

    // create params for the simulation
    SimulationProbabilities prob1{};
    SimulationProbabilities prob2{.5, .5};
    SimulationProbabilities prob3{.25, .25};
    SimulationProbabilities prob4{.1, 0.0};

    // init and populate parameters vector
    std::vector<SimulationParameters*> parameters = {
            create_params("sim1.txt", prob1, q_size_of_lobby, n_agents, n_infected_agents, n_negative_agents, who_knows_who, who_meets_who, n_steps),
            create_params("sim2.txt", prob2, q_size_of_lobby, n_agents, n_infected_agents, n_negative_agents, who_knows_who, who_meets_who, n_steps),
            create_params("sim3.txt", prob3, q_size_of_lobby, n_agents, n_infected_agents, n_negative_agents, who_knows_who, who_meets_who, n_steps),
            create_params("sim4.txt", prob4, q_size_of_lobby, n_agents, n_infected_agents, n_negative_agents, who_knows_who, who_meets_who, n_steps),
    };

    // run simulation
    std::cout << "starting simulations...\n";
    run_simulations(parameters);
    std::cout << "simulations finished...\n";

    return 0;
}
