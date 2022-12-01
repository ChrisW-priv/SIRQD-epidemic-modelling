#ifndef SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H
#define SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H

#include <cstdint>
#include <algorithm>

/// HERE WE CHOOSE OUR RANDOM 32 BIT INT CREATOR
/// for now we still use the default rand() function
/// TODO: change this later to better alternatives
#define rand32() ( rand() )

/// random float in range <0,1>
#define rand_float() ( ((float)rand32() / (float)0xffffffff) )

/// returns 32 bit int that is no bigger than range given
/// \param range max int that bounds the range
/// \return integer in range <0, range)
inline uint32_t random_bounded(uint32_t range) {
    uint64_t random32bit = rand32(); //32-bit random number
    uint64_t multiresult = random32bit * range;
    return multiresult >> 32;
}

/// uses R algorithm for reservoir sampling given a pointer to stream of data
/// \param stream pointer to first element
/// \param k number of elements chosen
/// \param cut_off limits number of elements taken from stream
/// \return pointer to first element chosen
template<typename T>
T* reservoir_sampling(const T* stream, size_t k, size_t cut_off)
{
    T* result = (T*) malloc(k*sizeof(T));

    size_t i=0, rand_index;
    while (i<k) result[i] = stream[i++];

    while (i < cut_off) {
        rand_index = random_bounded(i);
        if (rand_index < k) result[rand_index] = stream[i++];
    }
    return result;
}

/// uses R algorithm for reservoir sampling given a pointer to stream of data
/// \param begin Iterator of container
/// \param end Iterator of container
/// \param k number of elements chosen
/// \return pointer to first element chosen
template<typename T, typename Iter>
T* reservoir_sampling(Iter begin, Iter end, size_t k)
{
    T* result = (T*) malloc(k*sizeof(T));

    size_t i=0, rand_index;
    for (Iter curr = begin; curr!=end; ++curr){
        if (i<k) result[i] = *curr;
        rand_index = random_bounded(i);
        if (rand_index < k) result[rand_index] = *curr;
        ++i;
    }

    return result;
}

int weighted_choice(std::initializer_list<float> probabilities);

bool is_true(float probability);


#endif //SIRQD_EPIDEMIC_MODELLING_FAST_RANDOM_H
