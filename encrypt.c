#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <inttypes.h>
#define OPTIONS "vhi:o:n:"
#include <unistd.h>
#include <sys/stat.h>

void usage(char *exec) { /*usage error function which I reused from last assignment*/
    fprintf(stderr,
        "\n"
        "Synopsis:\n"
        "----encrypts message using public key"
        "\n"
        "USAGE\n %s./encrypt [-hv] [-n pubfile] -i infile -o output\n"
        "\n" /*all command line options listed*/
        "OPTIONS\n"
        "    -h        help and usage\n"
        "    -v        verbose mode\n"
        "    -n        specifies file containing public key (default rsa.pub)\n"
        "    -i        Specifies input file to encrypt (default stdin)\n"
        "    -o        specifies output file ot encrypt (default stdout)\n"
        "\n",
        exec);
    return;
}

int main(int argc, char **argv) {

    int opt;
    bool verbose = false;
    FILE *orig_message = stdin; //setting defaults
    FILE *encrypted_message = stdout;
    FILE *pub_key = fopen("rsa.pub.txt", "r");

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose = true; break;
        case 'i':
            orig_message = fopen(optarg, "r");
            if (orig_message == NULL) {
                fprintf(stderr, "ERROR: Nothing in File:\n"); //checking if file empty
                fprintf(stderr, "Try a Different File\n");
                return EXIT_FAILURE;
            }
            break;
        case 'o': encrypted_message = fopen(optarg, "w"); break;
        case 'n':
            fclose(pub_key);
            pub_key = fopen(optarg, "r");
            if (pub_key == NULL) {
                fprintf(stderr, "ERROR: Nothing in File\n"); //cheking if file empty
                return EXIT_FAILURE;
            }
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        }
    }

    mpz_t n, e, s, user;
    mpz_inits(n, e, s, user, NULL);
    char username[128];
    fseek(pub_key, 0, SEEK_SET); //resetting file pointer

    rsa_read_pub(n, e, s, username, pub_key); //reading from pubkey file

    if (verbose == true) {
        fprintf(stderr, "username = %s\n", username);
        gmp_fprintf(stderr, "s (%d bits)= %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_fprintf(stderr, "n (%d bits)= %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stderr, "e (%d bits)= %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    mpz_set_str(user, username, 62);

    if (rsa_verify(user, s, e, n) == false) { //verifying signiture
        fprintf(stderr, "ERROR: Unverified Signiture\n");
        return EXIT_FAILURE;
    }

    rsa_encrypt_file(orig_message, encrypted_message, n, e); //encrypting

    mpz_clears(n, e, s, user, NULL);
    fclose(orig_message);
    fclose(encrypted_message);
    fclose(pub_key);
    return 0;
}
