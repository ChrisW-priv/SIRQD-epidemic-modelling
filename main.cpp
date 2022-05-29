#include <iostream>
#include <thread>
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


struct SimulationProbabilities{
    float beta = 1.0; // probability of infection
    float mu = 1.0; // probability to recover
    float gamma = 1.0; // probability to enter quarantine
    float kappa = 1.0; // risk of death

    friend std::ostream& operator<<(std::ostream& stream, SimulationProbabilities& state){
        stream << "beta:" << state.beta << ", "
               << "mu:" << state.mu << ", "
               << "gamma:" << state.gamma << ", "
               << "kappa:" << state.kappa <<'\n';
        return stream;
    }
};


struct SimulationParameters{
    const char * out_file_name;
    SimulationProbabilities probabilities;
    uint16_t n_agents;
    uint16_t n_infected_agents;
    SortedSparseMatrix<uint16_t, uint32_t> & who_knows_who;
    SortedSparseMatrix<uint16_t, uint32_t> & who_meets_who;
    uint8_t n_steps;
};


struct SimulationState{
    uint16_t Susceptible = 0;
    uint16_t Infected = 0;
    uint16_t Recovered = 0;
    uint16_t Quarantined = 0;
    uint16_t Deceased = 0;

    /// modifies the state of Simulation state to match how many agents there are of given state
    void count_states(std::unique_ptr<Agent[]> & agents, uint16_t n_agents){
        uint16_t help_arr[5]{};

        for (uint16_t i=0;i<n_agents;++i)
            help_arr[agents[i].state] += 1;

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


void run_simulation(SimulationParameters* params) {
    const std::string& out_file_name = params->out_file_name;
    uint16_t n_agents = params->n_agents;
    uint16_t n_infected_agents = params->n_infected_agents;
    SimulationProbabilities & probabilities = params->probabilities;
    SortedSparseMatrix<uint16_t, uint32_t> & who_knows_who = params->who_knows_who;
    SortedSparseMatrix<uint16_t, uint32_t> & who_meets_who = params->who_meets_who;
    uint8_t n_steps = params->n_steps;

    // init agent array
    std::unique_ptr<Agent[]> agents{new Agent[n_agents]{}};

    // init next_state buffer
    std::unique_ptr<Agent[]> agent_next_step{new Agent[n_agents]{}};

    // populate with infected agents
    auto sim_state = SimulationState();
    populate_with_infected_agents(agents, n_agents, n_infected_agents);
    sim_state.count_states(agents, n_agents);

    // init log file
    std::ofstream out_file{out_file_name};

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
                    if (agents[n].state == State::Susceptible && is_true(probabilities.beta)) {
                        agent_next_step[n].state = State::Infected;
                    }
                }

                // check if agent recovers with probability mu
                if (is_true(probabilities.mu)) {
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


SimulationParameters* create_params(const char * out_file_name,
                                    SimulationProbabilities probabilities,
                                    uint16_t n_agents,
                                    uint16_t n_infected_agents,
                                    SortedSparseMatrix<uint16_t, uint32_t> & who_knows_who,
                                    SortedSparseMatrix<uint16_t, uint32_t> & who_meets_who,
                                    uint8_t n_steps)
{
    return new SimulationParameters
            {
                    out_file_name,
                    probabilities,
                    n_agents, n_infected_agents, who_knows_who, who_meets_who, n_steps
            };
}

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
