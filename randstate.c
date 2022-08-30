#include "randstate.h"

gmp_randstate_t state;

void randstate_init(uint64_t seed) {
    gmp_randinit_mt(state); //initializes state
    gmp_randseed_ui(state, seed); //sets random seed for use fo random number
}

void randstate_clear(void) {
    gmp_randclear(state); //clears memory
}
