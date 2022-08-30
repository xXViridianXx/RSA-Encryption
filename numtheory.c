#include "numtheory.h"

gmp_randstate_t state;

void gcd(mpz_t d, mpz_t a, mpz_t b) { //computes gcd
    mpz_t x, y;
    mpz_init_set(x, a);
    mpz_init_set(y, b);
    while (mpz_cmp_ui(y, 0) != 0) { //checks if b != 0
        mpz_set(d, y);
        mpz_mod(y, x, y); //sets remainder of a / b to b
        mpz_set(x, d); //sets a to b
    }
    mpz_set(d, x);
    mpz_clears(x, y, NULL);
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) { //calculates mod inverse
    mpz_t r, r_prime, r_temp;
    mpz_t t, t_prime, t_temp; //defining vars
    mpz_t q, product;

    mpz_init_set(r, n);
    mpz_init_set(r_prime, a); // set r = n and r' =a
    mpz_init_set_ui(t, 0);
    mpz_init_set_ui(t_prime, 1); //set t = 0 and t' = 1
    mpz_inits(r_temp, t_temp, product, q, NULL);

    while (mpz_cmp_ui(r_prime, 0) != 0) {
        mpz_tdiv_q(q, r, r_prime); //writing base a in terms of mod(n)
        mpz_set(r_temp, r);
        mpz_set(r, r_prime); //sets r to r'
        mpz_mul(product, q, r_prime); //multiplying r' and q
        mpz_sub(r_prime, r_temp, product);
        mpz_set(t_temp, t);
        mpz_set(t, t_prime); //setting t to t_prime
        mpz_mul(product, q, t_prime);
        mpz_sub(t_prime, t_temp, product); //set t' = t_temp - prod
    }
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(i, 0); //return no inverse
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n); //t+=n
    }

    mpz_set(i, t); //return the inverse

    mpz_clears(r, r_prime, r_temp, NULL);
    mpz_clears(t, t_prime, t_temp, NULL);
    mpz_clears(q, product, NULL);
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t v, p, e;
    mpz_init_set_ui(v, 1); //set v = 1
    mpz_init_set(p, base); //set p = base
    mpz_init_set(e, exponent); //stored expo in e because Miles said
    while (mpz_cmp_ui(e, 0) == 1) { //checks if d > 0
        if (mpz_odd_p(e) != 0) { //checks for odd exponent
            mpz_mul(v, v, p);
            mpz_mod(v, v, modulus); //if odd, v= (v*p) % (n)
        }
        mpz_mul(p, p, p); //does even exponent otherwise
        mpz_mod(p, p, modulus);
        mpz_tdiv_q_ui(e, e, 2);
    }
    mpz_set(out, v);
    mpz_clears(v, p, e, NULL);
}

bool is_prime(mpz_t n, uint64_t iters) { //checks if number prime
    if (mpz_odd_p(n) == 0) {
        return false;
    }

    mpz_t s, t, r, n_sub;
    mpz_inits(s, t, r, n_sub, NULL); //sets variable
    mpz_sub_ui(n_sub, n, 1);
    mpz_set(r, n_sub);

    while (mpz_odd_p(r) == 0) {
        mpz_add_ui(s, s, 1);
        mpz_tdiv_q_ui(r, r, 2); //write number 2^s * r where r is odd
    }

    mpz_t y, j, a, mod, random, s_sub, expo;
    mpz_inits(y, j, a, mod, random, s_sub, expo, NULL);
    mpz_set_ui(expo, 2); //sets variables

    mpz_sub_ui(mod, n, 4); //creates shift so number between [2, n-2]
    for (uint64_t i = 1; i < iters; i++) { //loop from 1 to #of iterations
        mpz_urandomm(random, state, n);
        mpz_mod(a, random, mod);
        mpz_add_ui(a, a, 2); //add 2 to finalize shift
        pow_mod(y, a, r, n); //use calculated r in power mod as exponent

        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_sub) != 0)) { //checks if y != 1 and y y!= n-1
            mpz_set_ui(j, 1);
            mpz_sub_ui(s_sub, s, 1);

            while ((mpz_cmp(j, s_sub) <= 0) && (mpz_cmp(y, n_sub) != 0)) {
                pow_mod(y, y, expo, n);
                if (mpz_cmp_ui(y, 1) == 0) { //if pow_mod output == 1, then false
                    mpz_clears(s, t, r, n_sub, NULL);
                    mpz_clears(y, j, a, mod, random, s_sub, expo, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, s_sub) != 0) { //if pow_mod output != n-1, output false
                mpz_clears(s, t, r, n_sub, NULL);
                mpz_clears(y, j, a, mod, random, s_sub, expo, NULL);
                return false;
            }
        }
    }
    mpz_clears(s, t, r, n_sub, NULL);
    mpz_clears(y, j, a, mod, random, s_sub, expo, NULL); //clear vars
    return true; //else true
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) { // makes prime number
    mpz_t rand_number, range, expo, shift, ran_shift, temp;
    mpz_init_set_ui(expo, 2);
    mpz_inits(rand_number, range, shift, temp, ran_shift, NULL); //init vars
    uint64_t bit_sub = bits - 1;
    mpz_urandomb(temp, state, bits + 5); //creating random number
    mpz_pow_ui(range, expo, bit_sub); // modulo = 2^{bits-1}

    mpz_urandomb(ran_shift, state, bits);
    mpz_mod(shift, ran_shift, range); //creating shift so inbetween 0-2^(n-1) shift
    mpz_add(rand_number, temp, shift); //adding random shift ot random number

    while (1) {
        if (is_prime(rand_number, iters)) { // if rand_number == prime, set p to rand_num
            mpz_set(p, rand_number);
            break;
        }

        mpz_urandomb(temp, state, bits + 5);
        mpz_urandomb(ran_shift, state, bits); //do same process again until prime achieved
        mpz_mod(shift, ran_shift, range);
        mpz_add(rand_number, temp, shift);
    }

    mpz_clears(rand_number, range, expo, shift, temp, ran_shift, NULL);
}
