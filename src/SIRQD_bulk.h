#ifndef SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H
#define SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H

#include <iostream>
#include "double_buffer.h"
#include "fast_random.h"


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


void populate_with_infected_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_infected, bool shuffle=true);
void populate_with_negative_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_negative, bool shuffle=true);
void populate_with_independent_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_independent, bool shuffle=true);

#endif //SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H
