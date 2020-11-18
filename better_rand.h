#ifndef _BETTER_RAND_H
#define _BETTER_RAND_H
#include <stdint.h>

#ifdef __cplusplus
using rng_t = std::mt19937 *;
using zipfian_dist_t = zipf_distribution<> *;
using uniform_dist_t = std::uniform_int_distribution<> *;
#else
struct rng;
struct zipfian_dist;
struct uniform_dist;
typedef struct rng *rng_t;
typedef struct zipfian_dist *zipfian_dist_t;
typedef struct uniform_dist *uniform_dist_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void gen_rand_bytes(char *start, char *end, rng_t rng);
void gen_rand_bytes_from_int(char *start, char *end, uint64_t seed);
rng_t new_rng(uint64_t seed);
void free_rng(rng_t rng);

// Generate an interger from [l, r].
zipfian_dist_t new_zipfian(uint64_t l, uint64_t r);
void free_zipfian(zipfian_dist_t dist);

uint64_t rand_from_zipfian(zipfian_dist_t dist, rng_t rng);

// Generate an interger from [l, r].
uniform_dist_t new_uniform(uint64_t l, uint64_t r);
void free_uniform(uniform_dist_t dist);

uint64_t rand_from_uniform(uniform_dist_t dist, rng_t rng);

#ifdef __cplusplus
}
#endif
#endif
