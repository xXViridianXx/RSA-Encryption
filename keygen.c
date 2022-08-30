#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <inttypes.h>
#define OPTIONS "hvb:c:n:d:s:"
#include <unistd.h>
#include <sys/stat.h>

void usage(char *exec) { /*usage error function which I reused from last assignment*/
    fprintf(stderr,
        "\n"
        "Synopsis:\n"
        "----generates public key and private key for encryption and decryption"
        "\n"
        "USAGE\n %s./keygen [-hv] [-b bits] -n pbfile -d pvfile\n"
        "\n" /*all command line options listed*/
        "OPTIONS\n"
        "    -h        help and usage\n"
        "    -v        verbose mode\n"
        "    -b        minimum bits for public key\n" //copied Eugene's usage prints.
        "    -c        iterations for testing prime\n" //same for encrypt and decypt as well
        "    -n        public key file\n"
        "    -d        private key file\n"
        "    -s        seed\n"
        "\n",
        exec);
    return;
}

int main(int argc, char **argv) {

    int opt;
    bool verbose = false; //sets val for verbose mode
    uint64_t nbits = 256; //default vals
    uint64_t iterations = 256;
    FILE *pubkey_out = fopen("rsa.pub.txt", "w");
    FILE *privkey_out = fopen("rsa.priv.txt", "w");
    uint64_t SEED;
    SEED = time(0); //setting seed

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose = true; break;
        case 'b': nbits = strtoul(optarg, NULL, 10); break;
        case 'c': iterations = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'n':
            fclose(pubkey_out);
            pubkey_out = fopen(optarg, "w");
            break;
        case 'd':
            fclose(privkey_out);
            privkey_out = fopen(optarg, "w");
            break;
        case 's': SEED = (uint64_t) strtoul(optarg, NULL, 10); break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        }
    }

    int permission = fileno(privkey_out); //getting file descriptor

    fchmod(permission, 0600); //setting permissions
    randstate_init(SEED);

    mpz_t p, q, n, e, d, user, signiture;
    mpz_inits(p, q, n, e, d, user, signiture, NULL);
    rsa_make_pub(p, q, n, e, nbits, iterations); //making public key
    rsa_make_priv(d, e, p, q); //making private key
    char *username = getenv("USERNAME"); //getting username from environment
    mpz_set_str(user, username, 62);
    rsa_sign(signiture, user, d, n);

    rsa_write_pub(n, e, signiture, username, pubkey_out);

    if (pubkey_out == NULL) { //checks if outputted NULL in pub_key
        fprintf(stderr, "ERROR: Nothing in File\n");
        fprintf(stderr, "Try a Different File\n");
        return EXIT_FAILURE;
    }

    rsa_write_priv(n, d, privkey_out); //checks if NULL in privKey
    if (privkey_out == NULL) {
        fprintf(stderr, "ERROR: Nothing in File\n");
        fprintf(stderr, "Try a Different File\n");

        return EXIT_FAILURE;
    }

    if (verbose == true) { //verbose mode applied
        fprintf(stderr, "username = %s\n", username);
        gmp_fprintf(stderr, "s (%d bits)    = %Zd\n", mpz_sizeinbase(signiture, 2), signiture);
        gmp_fprintf(stderr, "p (%d bits)    = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_fprintf(stderr, "q (%d bits)     = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_fprintf(stderr, "n (%d bits)    = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stderr, "e (%d bits)     = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_fprintf(stderr, "d (%d bits)    = %Zd\n", mpz_sizeinbase(d, 2), d);
    }
    fclose(pubkey_out);
    fclose(privkey_out);
    mpz_clears(p, q, n, e, d, user, signiture, NULL);
    randstate_clear();

    return 0;
}
