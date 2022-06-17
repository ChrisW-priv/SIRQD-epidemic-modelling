#include <iostream>
#include <thread>
#include "SIRQD_bulk.h"


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
    uint8_t opinion = 1; // 0 or 1
};


struct SimulationParameters{
    const char * out_file_name;
    SimulationProbabilities probabilities;
    uint8_t q_size_of_lobby;
    uint16_t n_agents;
    uint16_t n_infected_agents;
    SortedSparseMatrix<uint16_t, uint32_t> & who_knows_who;
    SortedSparseMatrix<uint16_t, uint32_t> & who_meets_who;
    uint8_t n_steps;
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
    void count_states(std::unique_ptr<Agent[]> & agents, uint16_t n_agents){
        uint16_t help_arr[5]{};
        NumberOfPositiveOpinions = 0;

        for (uint16_t i=0;i<n_agents;++i){
            help_arr[agents[i].state] += 1;
            NumberOfPositiveOpinions += agents[i].opinion;
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
               << state.NumberOfPositiveOpinions << ','
               << '\n';
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


SimulationParameters* create_params(const char * out_file_name,
                                    SimulationProbabilities probabilities,
                                    uint8_t q_size_of_lobby,
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
                    q_size_of_lobby,
                    n_agents, n_infected_agents, who_knows_who, who_meets_who, n_steps
            };
}


void run_simulation(SimulationParameters* params) {
    // init params for easier use later
    const std::string& out_file_name = params->out_file_name;
    uint16_t n_agents = params->n_agents;
    uint8_t q_size_of_lobby = params->q_size_of_lobby;
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

            // EPIDEMIC LAYER

            // handle states of the agent
            if (agent_now.state == State::Infected){
                // get neighbours
                auto neighbouring_indexes = who_meets_who.get_all_relations(i);
                // for each neighbour check if he will get infected
                for (auto n: neighbouring_indexes) {
                    // check if someone will get sick with probability beta
                    if (agents[n].state == State::Susceptible && is_true(probabilities.beta)) { // S -> I
                        agent_next_step[n].state = State::Infected;
                    }
                }

                // check if agent recovers with probability mu
                if (is_true(probabilities.mu)) { // I -> R
                    agents[i].state = State::Recovered;
                    agent_next_step[i].state = State::Recovered;
                }
                // check if agent enters quarantine with probability gamma
                else if (is_true(probabilities.gamma)) { // I -> Q
                    agents[i].state = State::Quarantined;
                    agent_next_step[i].state = State::Quarantined;
                }
                // check if agent dies with probability kappa
                else if (is_true(probabilities.kappa)){ // I -> D
                    agents[i].state = State::Deceased;
                    agent_next_step[i].state = State::Deceased;
                }
            }

            // OPINION LAYER

            if (is_true(agent_now.independence)) {
                auto neighbouring_indexes = who_knows_who.get_all_relations(i);
                uint16_t total_opinion = 0;
                uint16_t participants = 0;
                for (auto n: neighbouring_indexes){
                    if (agents[n].state != State::Deceased){
                        participants++;
                        total_opinion += agents[n].opinion;
                    }
                }
                if (participants < q_size_of_lobby) continue;

                if (total_opinion == 0) agent_next_step[i].opinion = 0;
                else if (total_opinion == q_size_of_lobby) agent_next_step[i].opinion = 1;

            } else { // doesn't act in conformity to the lobby
                if (is_true(0.5)) // flip opinion with fifty-fifty probability
                    agent_next_step[i].opinion = 1 - agents[i].opinion;
            }

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


void run_simulations(std::vector<SimulationParameters*>& parameters){
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
}
