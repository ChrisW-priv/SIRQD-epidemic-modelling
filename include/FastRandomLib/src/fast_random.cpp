#include "fast_random.h"
#include <algorithm>


double rd_float01(std::mt19937& generator){
    std::uniform_real_distribution<double> x(0.0, 1.0);
    return x(generator);
}

int weighted_choice(std::initializer_list<float> probabilities, std::mt19937& generator) {
    auto rnd_choice = rd_float01(generator);

    int index = 0;
    for (auto weight: probabilities) {
        if(rnd_choice < weight)
            return index;
        rnd_choice -= weight;
        index += 1;
    }
    return -1;
}

bool is_true(float probability, std::mt19937& generator){
    auto prob = rd_float01(generator);
    return prob < probability;
}
