#include "fast_random.h"
#include <cstdint>
#include <cstdlib>

/// following code is copied from stackoverflow answer:
/// https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
static uint32_t x=123456789, y=362436069, z=521288629;

/// returns random 32bit integer
uint32_t fast_randint32() { //period 2^96-1
    uint32_t t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return z;
}

/// END OF COPY PASTE


/// HERE WE CHOOSE OUR RANDOM 32 BIT INT CREATOR
/// for now we still use the default rand() function
/// TODO: change this later to better alternatives
#define rand32() ( rand() )


/// returns 32 bit int that is no bigger than range given
/// \param range max int that bounds the range
/// \return integer in range <0, range)
inline uint32_t random_bounded(uint32_t range) {
    uint64_t random32bit = rand32(); //32-bit random number
    uint64_t multiresult = random32bit * range;
    return multiresult >> 32;
}


/// returns index of choice taken basen on weights
int weighted_choice(const int* choice_weight, int n_choices){
    int sum_of_weights = 0;
    for(int i=0; i<n_choices; i++)
        sum_of_weights += choice_weight[i];

    uint32_t rnd = random_bounded(sum_of_weights);
    for(int i=0; i<n_choices; i++) {
        if(rnd < choice_weight[i])
            return i;
        rnd -= choice_weight[i];
    }
    return -1;
}


bool is_true(float probability){
    uint16_t n_expand = 0xffff;
    uint16_t random_choice = rand32();

    auto compare_to = (uint16_t) (probability * (float) n_expand) ;
    return random_choice < compare_to;
}
