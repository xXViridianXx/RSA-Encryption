#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

void gcd(mpz_t d, mpz_t a, mpz_t b);

void mod_inverse(mpz_t i, mpz_t a, mpz_t n);

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus);

bool is_prime(mpz_t n, uint64_t iters);

void make_prime(mpz_t p, uint64_t bits, uint64_t iters);

//void gcd(uint32_t *d, uint32_t a, uint32_t b);

//void mod_inverse(int32_t *i, int32_t a, int32_t n);

//void pow_mod(uint32_t *out, uint32_t base, uint32_t exponent, uint32_t modulus);

//bool is_prime(uint32_t n, uint64_t iters);

//void make_prime(uint32_t *p, uint32_t bits, uint64_t iters);
