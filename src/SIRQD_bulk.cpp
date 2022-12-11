#include <iostream>
#include <random>
#include "SIRQD_bulk.h"


void populate_with_infected_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_infected, std::mt19937 generator, bool shuffle)
{
    for (uint16_t i = 0; i < how_many_infected; ++i)
        agents[i].state = State::Infected;

    if (shuffle) std::shuffle(agents.begin(), agents.end(), generator);
}


void populate_with_negative_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_negative, std::mt19937 generator, bool shuffle)
{
    for (uint16_t i = 0; i < how_many_negative; ++i)
        agents[i].opinion = 0;

    if (shuffle) std::shuffle(agents.begin(), agents.end(), generator);
}

void populate_with_independent_agents(DoubleBuffer<Agent>& agents, uint16_t how_many_independent, std::mt19937 generator, bool shuffle)
{
    for (uint16_t i = 0; i < how_many_independent; ++i)
        agents[i].independence = 0;

    if (shuffle) std::shuffle(agents.begin(), agents.end(), generator);
}
