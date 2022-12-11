#ifndef SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H
#define SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H

#include <cstdint>
#include <algorithm>
#include <random>
#include <memory>


/// uses R algorithm for reservoir sampling given a pointer to stream of data
/// \param begin Iterator of container
/// \param end Iterator of container
/// \param k number of elements chosen
/// \return pointer to first element chosen
template<typename T, typename Iter>
std::unique_ptr<T[]> reservoir_sampling(Iter begin, Iter end, size_t k, std::mt19937 generator = std::mt19937{std::random_device{}()})
{
    std::unique_ptr<T[]> result{ new T[k] };

    size_t i=0, rand_index;
    Iter curr = begin;
    while (i<k) {
        result[i] = *curr;
        ++i; ++curr;
    }
    while (curr!=end){
        rand_index = std::uniform_int_distribution<uint32_t>{1,static_cast<uint32_t>(i)}(generator) - 1; // I want it <0-i) not <1,i>
        if (rand_index < k) result[rand_index] = *curr;
        ++i; ++curr;
    }

    return result;
}

int weighted_choice(std::initializer_list<float> probabilities, std::mt19937& generator);

bool is_true(float probability, std::mt19937& generator);

#endif //SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H
