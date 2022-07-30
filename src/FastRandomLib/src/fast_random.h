#ifndef SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H
#define SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H

#include <cstdint>
#include <algorithm>

uint32_t random_bounded(uint32_t range);

/// uses R algorithm for reservoir sampling given a pointer to stream of data
/// \param stream pointer to first element
/// \param k number of elements chosen
/// \param cut_off limits number of elements taken from stream
/// \return pointer to first element chosen
template<typename T, typename SIZE_TYPE_OF_INDEX>
T* fast_reservoir_sampling(const T* stream, SIZE_TYPE_OF_INDEX k, SIZE_TYPE_OF_INDEX cut_off)
{
    T* result = (T*) malloc(k*sizeof(int));

    SIZE_TYPE_OF_INDEX i=0, rand_index;
    while (i<k) result[i] = stream[i++];

    while (i < cut_off) {
        rand_index = fast_random_bounded(i);
        if (rand_index < k) result[rand_index] = stream[i++];
    }
    return result;
}

int weighted_choice(const int* choice_weight, int n_choices);
bool is_true(float probability);

template<typename T>
T& random_shuffle(T& array, uint16_t n_elements) {
    for (uint16_t i = n_elements; i > 1; i--) {
        auto p = random_bounded(i); // number in [0,i)

        std::swap(array[i - 1], array[p]); // swap the values at i-1 and p
    }
    return array;
}

#endif //SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H
