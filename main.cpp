#include <iostream>
#include <thread>
#include "SortedSparseMatrix.h"
#include "SIRQD_bulk.cpp"


int main() {
    // init parameters of the simulation
    constexpr uint16_t n_agents = 10'000;
    constexpr uint8_t n_steps = 50;
    constexpr uint16_t n_infected_agents = 500;

    // init adjacency matrix
    SortedSparseMatrix who_knows_who{n_agents, 10 * n_agents};
    SortedSparseMatrix who_meets_who{n_agents, 10 * n_agents};

    // import relations between agents from file
    std::cout << "importing relations from files...\n";
    who_knows_who.import_relations_from_file("../who_knows_who.txt");
    who_meets_who.import_relations_from_file("../who_meets_who.txt");

    // create params for the simulation

    SimulationProbabilities prob1{};
    SimulationProbabilities prob2{.5, .5};
    SimulationProbabilities prob3{.25, .25};
    SimulationProbabilities prob4{.1, 0.0};

    std::vector<SimulationParameters*> parameters = {
            create_params("sim1.txt", prob1, n_agents, n_infected_agents, who_knows_who, who_meets_who, n_steps),
            create_params("sim2.txt", prob2, n_agents, n_infected_agents, who_knows_who, who_meets_who, n_steps),
            create_params("sim3.txt", prob3, n_agents, n_infected_agents, who_knows_who, who_meets_who, n_steps),
            create_params("sim4.txt", prob4, n_agents, n_infected_agents, who_knows_who, who_meets_who, n_steps),
    };

    // init vector of worker threads
    std::vector<std::thread> threads;

    // start treads with parameters specified
    for (auto & parameter : parameters) {
        threads.emplace_back(run_simulation, parameter);
    }

    // wait for the end of execution of all worker threads
    for (auto &th : threads) {
        th.join();
    }

    return 0;
}
