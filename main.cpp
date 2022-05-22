#include <iostream>
#include "SortedSparseMatrix.h"

enum State{
    Susceptible,
    Infected,
    Recovered,
    Quarantined,
    Deceased
};


struct Agent{
    State state = State::Susceptible;
    float independence = 0.0;
    float opinion = 1.0;
};


struct SimulationParameters{
    float beta = 1.0; // probability of infection
    float mu = 1.0; // probability to recover
    float gamma = 1.0; // probability to enter quarantine
    float kappa = 1.0; // risk of death
};


/// TODO: make better function, this is just quick implementation!!
inline bool is_true(float probability){
    int r = rand();
    int n = 10'000;
    int x = r % n;
    int compare_to = (int) (probability * (float) n) ;
    return x < compare_to;
}


int main() {
    // init parameters of the simulation
    constexpr SimulationParameters params{};
    constexpr uint16_t n_agents = 10'000;
    constexpr uint8_t n_steps = 50;

    // init agents
    Agent agents[n_agents]{};
    Agent agent_next_step[n_agents]{};

    // TODO: populate matrix with infected agents:
    // here

    // init adjacency matrix
    SortedSparseMatrix who_knows_who{n_agents, n_agents};
    SortedSparseMatrix who_meets_who{n_agents, n_agents};

    // init log file
    std::ofstream out_file{"sim_state_file.txt"};

    // start simulation
    int sim_step_number=0;
    while (sim_step_number < n_steps) {
        // simulation step:
        for (uint16_t i = 0; i < n_agents; i++) {
            auto agent_now = agents[i];

            // possibly not needed if we do everything right!! Left for now to make sure it's handled correctly
            agent_next_step[i] = agents[i];

            // handle states of the agent
            if (agent_now.state == State::Infected){
                // get neighbours
                auto neighbouring_indexes = who_meets_who.get_all_relations(i);
                for (auto n: neighbouring_indexes) {
                    // check if someone will get sick with probability beta
                    if (is_true(params.beta)) agent_next_step[n].state = State::Infected;
                }

                // check if agent recovers with probability mu
                if (is_true(params.beta)) agent_next_step[i].state = State::Recovered;
            }

            // TODO: update opinions of each agent in next step:
            // here
        }

        // TODO: log data of simulation step changes
        out_file << "test step: " << sim_step_number << '\n';

        // swap buffers for next step
        Agent *left = agents;
        Agent *right = agent_next_step;

        auto* swap = left;
        left = right;
        right = swap;

        // go to next sim step
        sim_step_number++;
    };

    return 0;
}
