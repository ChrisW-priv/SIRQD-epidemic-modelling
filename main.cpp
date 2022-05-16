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


inline int sim_step(int sim_step_number, const SimulationParameters& params, Agent agents[]) {
    /* what do we wanna do here?? */
    return 0;
}


int main() {
    constexpr SimulationParameters params{};
    constexpr uint32_t n_agents = 10'000;
    constexpr uint8_t n_steps = 50;

    Agent agents[n_agents]{};

    SortedSparseMatrix who_knows_who{n_agents, n_agents};
    SortedSparseMatrix who_meets_who{n_agents, n_agents};

    int sim_step_number=0;
    while (sim_step_number < n_steps) sim_step(sim_step_number++, params, agents);

    return 0;
}
