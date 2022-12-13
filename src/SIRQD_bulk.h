#ifndef SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H
#define SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H

#include <iostream>
#include <random>
#include "../include/double_buffer/src/double_buffer.h"
#include "../include/FastRandomLib/src/fast_random.h"
#include "../include/SortedSparseMatrix/src/SortedSparseMatrix.h"


enum State{
    Susceptible,
    Infected,
    Recovered,
    Quarantined,
    Deceased
};


struct Agent{
    State state = State::Susceptible;
    float independence = 1.0;
    uint8_t opinion = 1; // 0 or 1
};


struct SimulationState{
    // epidemic layer data
    uint16_t Susceptible = 0;
    uint16_t Infected = 0;
    uint16_t Recovered = 0;
    uint16_t Quarantined = 0;
    uint16_t Deceased = 0;

    // opinion layer data
    uint16_t NumberOfPositiveOpinions = 0;

    /// modifies the state of Simulation state to match how many agents there are of given state
    void count_states(DoubleBuffer<Agent>& agents){
        uint16_t help_arr[5]{};
        NumberOfPositiveOpinions = 0;

        for (auto agent: agents){
            help_arr[agent.state] += 1;
            NumberOfPositiveOpinions += agent.opinion;
        }

        Susceptible = help_arr[0];
        Infected = help_arr[1];
        Recovered = help_arr[2];
        Quarantined = help_arr[3];
        Deceased = help_arr[4];
    }

    friend std::ostream& operator<<(std::ostream& stream, SimulationState state){
        stream << state.Susceptible << ','
               << state.Infected << ','
               << state.Recovered << ','
               << state.Quarantined << ','
               << state.Deceased << ','
               << state.NumberOfPositiveOpinions
               << '\n';
        return stream;
    }
};


void populate_with_infected_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_infected, std::mt19937 generator, bool shuffle=true);
void populate_with_negative_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_negative, std::mt19937 generator, bool shuffle=true);
void populate_with_independent_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_independent, std::mt19937 generator, bool shuffle=true);


void do_computation(int nunber_of_agents,  int n_steps, int n_infected_agents,  int n_negative_agents,
                    int n_independent_agents,  int q_size_of_lobby, const char *import_filename_opinion,
                    const char *import_filename_epidemic, const char *log_file_name, float probability_of_infection,
                    float probability_of_recovery,  float probability_of_quarantine,  float probability_of_death, int seed){

    // Create a random number generator
    std::mt19937 generator(seed);

    auto n_agents = (size_t) nunber_of_agents;
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
    SortedSparseMatrix<uint16_t, uint32_t> opinion_layer_graph{n_agents, n_agents};
    SortedSparseMatrix<uint16_t, uint32_t> epidemic_layer_graph{n_agents, n_agents};

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


#endif //SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H
