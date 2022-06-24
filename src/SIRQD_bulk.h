#ifndef SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H
#define SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H

#include <iostream>
#include "SortedSparseMatrix.h"
struct SimulationParameters;


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

SimulationParameters *
create_params(const char *out_file_name, SimulationProbabilities probabilities, uint8_t q_size_of_lobby,
              uint16_t n_agents, uint16_t n_infected_agents, uint16_t n_negative_agents,
              SortedSparseMatrix<uint16_t, uint32_t> &who_knows_who,
              SortedSparseMatrix<uint16_t, uint32_t> &who_meets_who, uint8_t n_steps);

void run_simulations(std::vector<SimulationParameters*>& parameters);


#endif //SIRQD_EPIDEMIC_MODELLING_SIRQD_BULK_H
