#include "fast_random.h"
#include <cstdint>
#include <cstdlib>
#include <algorithm>


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


int weighted_choice(std::initializer_list<float> probabilities) {
    float weight_sum = 0;
    for (auto weight: probabilities) weight_sum += weight;

    float rnd_choice = rand_float();

    int index = 0;
    for (auto weight: probabilities) {
        if(rnd_choice < weight)
            return index;
        rnd_choice -= weight;
        index += 1;
    }
    return -1;
}

bool is_true(float probability){
    uint16_t n_expand = 0xffff;
    uint16_t random_choice = rand32();

    auto compare_to = (uint16_t) (probability * (float) n_expand) ;
    return random_choice < compare_to;
}
