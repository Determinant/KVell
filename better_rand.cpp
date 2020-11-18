#include <random>
#include "zipf.h"
#include "better_rand.h"

extern "C" {

void gen_rand_bytes(char *start, char *end, rng_t rng) {
    std::uniform_int_distribution<> dis;
    while (start != end)
        *start++ = dis(*rng);
}

void gen_rand_bytes_from_int(char *start, char *end, uint64_t seed) {
    std::mt19937 rng{seed};
    std::uniform_int_distribution<> dis;
    while (start != end)
        *start++ = dis(rng);
}

rng_t new_rng(uint64_t seed) {
    return new std::mt19937(seed);
}

// Generate an interger from [l, r].
zipfian_dist_t new_zipfian(uint64_t l, uint64_t r) {
    return new zipf_distribution<>(l, r);
}

uint64_t rand_from_zipfian(zipfian_dist_t dist, rng_t rng) {
    return (*dist)(*rng);
}

// Generate an interger from [l, r].
uniform_dist_t new_uniform(uint64_t l, uint64_t r) {
    return new std::uniform_int_distribution<>(l, r);
}

uint64_t rand_from_uniform(uniform_dist_t dist, rng_t rng) {
    return (*dist)(*rng);
}

void free_rng(rng_t rng) {
    delete rng;
}

void free_zipfian(zipfian_dist_t dist) {
    delete dist;
}

void free_uniform(uniform_dist_t dist) {
    delete dist;
}

}
