#include "rsa.h"
#include "numtheory.h"

void lambda(mpz_t lamb_n, mpz_t p, mpz_t q) { //will use for rsa
    mpz_t gcd_out, psub, qsub, phi;
    mpz_inits(psub, qsub, gcd_out, phi, NULL); //init vars

    mpz_sub_ui(psub, p, 1); //get p-1 and q-1
    mpz_sub_ui(qsub, q, 1);
    mpz_mul(phi, psub, qsub); //caculate phi
    gcd(gcd_out, psub, qsub); //plug in gcd
    mpz_abs(phi, phi);

    mpz_fdiv_q(lamb_n, phi, gcd_out); //divide and set to lamb_n

    mpz_clears(gcd_out, psub, qsub, phi, NULL);
}

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) { //ask Miles
    mpz_set_ui(e, 65537);
    uint64_t ran = random();
    uint64_t range = (ran % (nbits / 2));
    uint64_t p_bits = (range + (nbits / 4)); //sets range from 1/4 - 3/4
    uint64_t q_bits;

    q_bits = nbits - p_bits; //sets rest of bits to q_bits

    make_prime(p, p_bits, iters); //make p prime number
    make_prime(q, q_bits, iters); //make q prime number
    mpz_mul(n, p, q);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n); //writes pubilc key to file suggested by user
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}
void rsa_read_pub(
    mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) { //read from file suggested by user
    mpz_t f;
    mpz_init(f);
    fseek(pbfile, 0, SEEK_SET);
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);

    fscanf(pbfile, "%s\n", username);
    mpz_clear(f);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) { //makes d and e using lambda
    mpz_t lamb_n;
    mpz_init(lamb_n);

    lambda(lamb_n, p, q); //use of lambda funtion
    mod_inverse(d, e, lamb_n);
    mpz_clear(lamb_n);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) { //writes priv key to file
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) { //reads priv key from file
    mpz_t f;
    mpz_init(f);
    gmp_fscanf(pvfile, "%Zx\n", &f);
    mpz_set(n, f);
    gmp_fscanf(pvfile, "%Zx\n", &f);
    mpz_set(d, f);
    mpz_clear(f);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

void rsa_encrypt_file(
    FILE *infile, FILE *outfile, mpz_t n, mpz_t e) { //coded using help from Audrey
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    uint32_t k = (mpz_sizeinbase(n, 2) - 1) / 8; //gets size of n in bytes
    uint32_t j = sizeof(uint8_t);
    size_t read;

    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t)); //creates a block to read in chunks
    block[0] = 0xFF; //adds FF to front
    while (feof(infile) == 0) { //while not end of file
        read = fread(block + 1, j, k - 1, infile); //go to first block and store contents size of j
        mpz_import(m, read + 1, 1, sizeof(uint8_t), 1, 0, block); //convert to numbers
        rsa_encrypt(c, m, e, n); //encrypt
        gmp_fprintf(outfile, "%Zx\n", c); //print to outfile
    }

    free(block);
    mpz_clears(c, m, NULL);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) { //decrypt using pow_mod
    pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) { //inverse of rsa_encrypt
    mpz_t c, m;
    mpz_inits(c, m, NULL);
    uint32_t k = (mpz_sizeinbase(n, 2) - 1) / 8; //same size in bytes
    uint32_t j = sizeof(uint8_t);
    size_t size;

    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t));
    while (feof(infile) == 0) {
        gmp_fscanf(infile, "%Zx\n", c);
        rsa_decrypt(m, c, d, n);
        mpz_export(block, &size, 1, j, 1, 0, m); //stores size in &size which used for writing
        fwrite(block + 1, j, size - 1, outfile); //goes to first block because we don't want FF
    } // coded using help for Brian Mak

    free(block);
    mpz_clears(c, NULL);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n); //signing
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t; //verify signiture
    mpz_init(t);
    pow_mod(t, s, e, n);

    if (mpz_cmp(t, m) == 0) { //if signiture mathces, return true and continue process
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false;
}
