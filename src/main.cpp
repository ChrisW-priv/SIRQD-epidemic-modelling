#include <iostream>
#include "SIRQD_bulk.h"


int main() {
    // init parameters of the simulation
    constexpr uint16_t n_agents = 10'000;
    constexpr uint8_t n_steps = 25;
    constexpr uint16_t n_infected_agents = 500;
    constexpr uint16_t n_negative_agents = 500;
    constexpr uint16_t n_independent_agents = 500;
    constexpr uint8_t q_size_of_lobby = 10;
    const char * import_filename_opinion{"../data/who_knows_who1.txt" };
    const char * import_filename_epidemic{ "../data/who_meets_who1.txt" };
    const char * log_file_name{"C:/Users/Chris/SIRQD-epidemic-modelling/results/sim1.txt"};
    constexpr float probability_of_infection = 0.5;
    constexpr float probability_of_recovery = 0.5;
    constexpr float probability_of_quarantine = 0;
    constexpr float probability_of_death = 0;
    constexpr int seed=10;

    do_computation(n_agents, n_steps, n_infected_agents, n_negative_agents, n_independent_agents, q_size_of_lobby,
        import_filename_opinion, import_filename_epidemic, log_file_name, probability_of_infection,
        probability_of_recovery, probability_of_quarantine, probability_of_death, seed);
}
