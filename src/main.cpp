#include <iostream>
#include <random>
#include "SortedSparseMatrix.h"
#include "SIRQD_bulk.h"
#include "double_buffer.h"
#include "fast_random.h"


void do_computation(uint16_t n_agents,  uint8_t n_steps, uint16_t n_infected_agents,  uint16_t n_negative_agents,
         uint16_t n_independent_agents,  uint8_t q_size_of_lobby, const char *import_filename_opinion,
         const char *import_filename_epidemic, const char *log_file_name, float probability_of_infection,
         float probability_of_recovery,  float probability_of_quarantine,  float probability_of_death, int seed);

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

void do_computation(uint16_t n_agents,  uint8_t n_steps, uint16_t n_infected_agents,  uint16_t n_negative_agents,
        uint16_t n_independent_agents,  uint8_t q_size_of_lobby, const char *import_filename_opinion,
        const char *import_filename_epidemic, const char *log_file_name, float probability_of_infection,
        float probability_of_recovery,  float probability_of_quarantine,  float probability_of_death, int seed){

    // Create a random number generator
    std::mt19937 generator(seed);

    // init agent array
    DoubleBuffer<Agent> agents{n_agents};

    //change states of the agents depending on the init values of the simulation
    populate_with_infected_agents(agents, n_infected_agents, generator, true);
    populate_with_negative_agents(agents, n_negative_agents, generator, true);
    populate_with_independent_agents(agents, n_independent_agents, generator, true);

    // at the beginning buffers need to match to avoid cyclic state swap
    agents.match_buffers();

    auto sim_state = SimulationState();
    sim_state.count_states(agents);

    // init log file
    std::ofstream log_file{log_file_name};
    if (!log_file) { std::cout << "couldn't open file to write to!\n"; exit(-1); }
    log_file << "SimStep,Susceptible,Infected,Recovered,Quarantined,Deceased,NumberOfPositiveOpinions\n";
    log_file << 0 << ',' << sim_state;

    // init matrices of relations
    SortedSparseMatrix opinion_layer_graph{n_agents, n_agents};
    SortedSparseMatrix epidemic_layer_graph{n_agents, n_agents};

    // import relations between agents from file
    std::cout << "importing relations from files...\n";
    opinion_layer_graph.import_sorted_relations_from_file(import_filename_opinion);
    epidemic_layer_graph.import_sorted_relations_from_file(import_filename_epidemic);
    std::cout << "done importing relations\n";

    // start simulation
    uint16_t sim_step_number=1;
    while (sim_step_number <= n_steps) {
        // simulation step:
        for (uint16_t agent_index = 0; agent_index < n_agents; agent_index++) {
            auto agent_now = agents[agent_index];
            agents.at_next(agent_index) = agent_now;

            // if current agent is dead - continue to the next agent, no need to handle him
            if (agent_now.state == Deceased) continue;

            // EPIDEMIC LAYER

            // handle states of the agent
            if (agent_now.state == Infected){
                // infection spread changes based on opinion of the carrier
                auto infection_spread_risk = agent_now.opinion == 1 ? probability_of_infection : probability_of_infection/2;

                // get neighbours
                auto neighbouring_indexes = epidemic_layer_graph.get_all_relations(agent_index);

                // for each neighbour check if he will get infected
                for (auto n: neighbouring_indexes) {
                    // check if someone will get sick based on infection probability

                    // S -> I
                    if (agents[n].state == Susceptible && is_true(infection_spread_risk, generator))
                        agents.at_next(n).state = Infected;
                }

                std::initializer_list<float> list{
                    1 - (probability_of_recovery + probability_of_quarantine + probability_of_death),
                    probability_of_recovery, probability_of_quarantine, probability_of_death
                };
                switch (weighted_choice(list, generator)) {
                    case (0):
                        // state doesn't change
                        agents.at_next(agent_index).state = Infected;
                        break;
                    case (1):
                        agents.at_next(agent_index).state = Recovered;
                        break;
                    case(2):
                        agents.at_next(agent_index).state = Quarantined;
                        break;
                    case(3):
                        agents.at_next(agent_index).state = Deceased;
                        break;
                }
            }

            // OPINION LAYER

            if (is_true(agent_now.independence, generator)) { // acts in conformity to the lobby
                auto neighbouring_indexes = opinion_layer_graph.get_all_relations(agent_index);

                // first - filter all dead agents to simplify things later (dead people can't influence opinions)
                std::vector<uint16_t> filtered_agents;
                std::copy_if(neighbouring_indexes.begin(), neighbouring_indexes.end(),
                             std::back_inserter(filtered_agents),
                             [&agents](uint16_t index){ return agents[index].state != Deceased; } );

                if (filtered_agents.size() < q_size_of_lobby) continue; // skip if too little neighbours

                uint16_t total_opinion = 0;
                if (filtered_agents.size() == q_size_of_lobby) { // if number of alive agents is equal then no need to be fancy
                    for (auto n: filtered_agents) total_opinion += agents[n].opinion;
                } else { // fancy sampling method used here
                    auto result = reservoir_sampling<uint16_t>(filtered_agents.begin(), filtered_agents.end(),
                                                               q_size_of_lobby, generator);

                    for (int i = 0; i < q_size_of_lobby; ++i) {
                        auto agent_chosen_in_lobby = agents.at_curr(result[i]);
                        total_opinion += agent_chosen_in_lobby.opinion;
                    }
                }

                if (total_opinion == 0) agents.at_next(agent_index).opinion = 0;
                else if (total_opinion == q_size_of_lobby) agents.at_next(agent_index).opinion = 1;

            } else { // doesn't act in conformity to the lobby
                if (is_true(0.5, generator)) // flip opinion with fifty-fifty probability
                    agents.at_next(agent_index).opinion = 1 - agents[agent_index].opinion;
            }

        }
        // swap buffers for next step
        agents.swap_buffers();

        // get number of agents in each state
        sim_state.count_states(agents);

        // log step state to file
        log_file << sim_step_number << ',' << sim_state;

        // go to next sim step
        sim_step_number++;
    }
}
