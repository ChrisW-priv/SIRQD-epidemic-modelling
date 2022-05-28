#include <iostream>
#include <memory>
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

    /// modifies the state of Simulation state to match how many agents there are of given state
    void count_states(std::unique_ptr<Agent[]> & agents, uint16_t n_agents){
        this->Susceptible=0;
        this->Infected=0;
        this->Recovered=0;
        this->Quarantined=0;
        this->Deceased=0;

        for (uint16_t i=0;i<n_agents;++i){
            switch (agents[i].state) {
                case State::Susceptible:
                    this->Susceptible+=1;
                    break;
                case State::Infected:
                    this->Infected+=1;
                    break;
                case State::Recovered:
                    this->Recovered+=1;
                    break;
                case State::Quarantined:
                    this->Quarantined+=1;
                    break;
                case State::Deceased:
                    this->Deceased+=1;
                    break;
            }
        }
    }

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
    uint16_t base = 0;
    uint16_t pos;
    uint16_t step = n_agents/how_many_infected;
    uint16_t chunk = 0;
    while (chunk < how_many_infected){
        pos = base + rand_int(step);
        agents[pos].state = State::Infected;
        base += step;
        ++chunk;
    }
}


void run_simulation(uint16_t n_agents,
                    uint16_t n_infected_agents,
                    SimulationParameters & params,
                    SortedSparseMatrix<uint16_t, uint32_t> & who_knows_who,
                    SortedSparseMatrix<uint16_t, uint32_t> & who_meets_who,
                    uint8_t n_steps) {
    // init agent array
    std::unique_ptr<Agent[]> agents{new Agent[n_agents]{}};

    // init next_state buffer
    std::unique_ptr<Agent[]> agent_next_step{new Agent[n_agents]{}};

    // populate with infected agents
    auto sim_state = SimulationState();
    populate_with_infected_agents(agents, n_agents, n_infected_agents);
    sim_state.count_states(agents, n_agents);

    // init log file
    std::ofstream out_file{"sim_state_file.txt"};

    out_file << "SimStep,Susceptible,Infected,Recovered,Quarantined,Deceased\n";
    out_file << 0 << ',' << sim_state;

    // start simulation
    uint16_t sim_step_number=1;
    while (sim_step_number <= n_steps) {
        // simulation step:
        for (uint16_t i = 0; i < n_agents; i++) {
            auto agent_now = agents[i];

            // handle states of the agent
            if (agent_now.state == State::Infected){
                // get neighbours
                auto neighbouring_indexes = who_meets_who.get_all_relations(i);
                // for each neighbour check if he will get infected
                for (auto n: neighbouring_indexes) {
                    // check if someone will get sick with probability beta
                    if (agents[n].state == State::Susceptible && is_true(params.beta)) {
                        agent_next_step[n].state = State::Infected;
                    }
                }

                // check if agent recovers with probability mu
                if (is_true(params.mu)) {
                    agents[i].state = State::Recovered;
                    agent_next_step[i].state = State::Recovered;
                }
            }

            // TODO: update opinions of each agent in next step:
            // here
        }
        // swap buffers for next step
        agent_next_step.swap(agents);

        // get number of agents in each state
        sim_state.count_states(agents, n_agents);

        // log step state to file
        out_file << sim_step_number << ',' << sim_state;

        // go to next sim step
        sim_step_number++;
    }
}


int main() {
    // init parameters of the simulation
    SimulationParameters params{};
    constexpr uint16_t n_agents = 10'000;
    constexpr uint8_t n_steps = 50;
    constexpr uint16_t n_infected_agents = 500;

    // init agent array
    std::unique_ptr<Agent[]> agents{new Agent[n_agents]{}};

    // init next_state buffer
    std::unique_ptr<Agent[]> agent_next_step{new Agent[n_agents]{}};

    // init adjacency matrix
    SortedSparseMatrix who_knows_who{n_agents, 10 * n_agents};
    SortedSparseMatrix who_meets_who{n_agents, 10 * n_agents};

    // init relations
    std::cout << "importing relations from files...\n";
    who_knows_who.import_relations_from_file("../who_knows_who.txt");
    who_meets_who.import_relations_from_file("../who_meets_who.txt");

    // do calculations
    run_simulation(n_agents, n_infected_agents, params, who_knows_who, who_meets_who, n_steps);

    return 0;
}
