#include <iostream>
#include <memory>
#include <cstring>
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


struct SimulationState{
    uint16_t Susceptible = 10'000;
    uint16_t Infected = 0;
    uint16_t Recovered = 0;
    uint16_t Quarantined = 0;
    uint16_t Deceased = 0;

    friend std::ostream& operator<<(std::ostream& stream, SimulationState state){
        stream << state.Susceptible << ','
                << state.Infected << ','
                << state.Recovered << ','
                << state.Quarantined << ','
                << state.Deceased <<'\n';
        return stream;
    }
};

inline uint16_t rand_int(uint16_t max_range=0xffff){
    return rand() % max_range;
}

inline bool is_true(float probability){
    uint16_t n = 0xffff;
    uint16_t x = rand_int(n);
    int compare_to = (int) (probability * (float) n) ;
    return x < compare_to;
}


void populate_with_infected_agents(std::unique_ptr<Agent[]> & agents, uint16_t n_agents, uint16_t how_many_infected){
    std::vector<bool> changed;
    changed.resize(n_agents);
    std::fill(changed.begin(), changed.end(), false);

    for (int i = 0; i < how_many_infected; ++i) {
        uint16_t next_pos = rand_int(n_agents);
        if (changed[next_pos]) continue;
        changed[next_pos] = true;
        agents[next_pos].state = Infected;
    }
}


int main() {
    // init parameters of the simulation
    constexpr SimulationParameters params{};
    constexpr uint16_t n_agents = 10'000;
    constexpr uint8_t n_steps = 50;
    constexpr uint16_t n_infected_agents = 500;

    // init agent array
    std::unique_ptr<Agent[]> agents{new Agent[n_agents]};
    for (int i = 0; i < n_agents; ++i) {
        agents[i] = Agent{};
    }

    // init next_state buffer
    std::unique_ptr<Agent[]> agent_next_step{new Agent[n_agents]};

    // populating with infected agents
    std::cout << "populating with infected agents...\n";
    auto sim_state = SimulationState();
    sim_state.Infected = n_infected_agents;
    sim_state.Susceptible -= n_infected_agents;
    populate_with_infected_agents(agents, n_agents, n_infected_agents);

    // init adjacency matrix
    SortedSparseMatrix who_knows_who{n_agents, 10 * n_agents};
    SortedSparseMatrix who_meets_who{n_agents, 10 * n_agents};

    // init relations
    std::cout << "importing relations from files...\n";
    who_knows_who.import_relations_from_file("../who_knows_who.txt");
    who_meets_who.import_relations_from_file("../who_meets_who.txt");

    // init log file
    std::cout << "opening log file...\n";
    std::ofstream out_file{"sim_state_file.txt"};

    out_file << "SimStep,Susceptible,Infected,Recovered,Quarantined,Deceased\n";
    out_file << 0 << ',' << sim_state;

    // start simulation
    std::cout << "starting simulation...\n";
    uint16_t sim_step_number=1;
    while (sim_step_number <= n_steps) {
        // simulation step:
        std::cout << "started processing step " << sim_step_number << '\n';
        for (uint16_t i = 0; i < n_agents; i++) {
            auto agent_now = agents[i];

            // handle states of the agent
            if (agent_now.state == State::Infected){
                // get neighbours
                auto neighbouring_indexes = who_meets_who.get_all_relations(i);
                for (auto n: neighbouring_indexes) {
                    if (agent_next_step[n].state == State::Infected) continue;
                    // check if someone will get sick with probability beta
                    if (agents[n].state == State::Susceptible && is_true(params.beta)) {
                        agent_next_step[n].state = State::Infected;
                        sim_state.Susceptible -= 1;
                        sim_state.Infected += 1;
                    }
                }

                // check if agent recovers with probability mu
                if (is_true(params.mu)) {
                    agent_next_step[i].state = State::Recovered;
                    sim_state.Infected -= 1;
                    sim_state.Recovered += 1;
                }
            }

            // TODO: update opinions of each agent in next step:
            // here
        }

        // log step state to file
        out_file << sim_step_number << ',' << sim_state;

        // swap buffers for next step
        agent_next_step.swap(agents);

        // go to next sim step
        sim_step_number++;
    };

    std::cout << "simulation finished!\n";

    return 0;
}
