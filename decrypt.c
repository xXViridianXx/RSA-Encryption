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
        "----decrypts message using private key"
        "\n"
        "USAGE\n %s./decrypt [-hv] [-n privfile] -i infile -o output\n"
        "Remember to decrypt and encrypted file--otherwise bad things will happen!\n"
        "\n" /*all command line options listed*/
        "OPTIONS\n"
        "    -h        help and usage\n"
        "    -v        verbose mode\n"
        "    -n        specifies file containing private key (default rsa.priv)\n"
        "    -i        Specifies input file to decrypt (default stdin)\n"
        "    -o        specifies output file ot decrypt (default stdout)\n"
        "\n",
        exec);
    return;
}

int main(int argc, char **argv) {

    int opt;
    bool verbose = false;
    FILE *encrypted_message = stdin; //setting defaults
    FILE *orig_message = stdout;
    FILE *priv_key = fopen("rsa.priv.txt", "r");

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose = true; break;
        case 'i':
            encrypted_message = fopen(optarg, "r");
            if (encrypted_message == NULL) { //checking if inputted file empty
                fprintf(stderr, "ERROR: Nothing in File");
                return EXIT_FAILURE;
            }
            break;
        case 'o': orig_message = fopen(optarg, "w"); break;
        case 'n':
            fclose(priv_key);
            priv_key = fopen(optarg, "r");
            if (priv_key == NULL) { //checking if inputed file empty
                fprintf(stderr, "ERROR: Nothing in File");
                return EXIT_FAILURE;
            }
            break;
        default:
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        }
    }

    mpz_t n, d;
    mpz_inits(n, d, NULL);
    fseek(priv_key, 0, SEEK_SET);
    fseek(encrypted_message, 0, SEEK_SET); //resetting file pointer

    rsa_read_priv(n, d, priv_key); //reading from priv key file

    if (verbose == true) {
        gmp_fprintf(stderr, "n (%d bits)= %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stderr, "d (%d bits)= %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    rsa_decrypt_file(encrypted_message, orig_message, n, d); //decrypting

    mpz_clears(n, d, NULL);
    fclose(orig_message);
    fclose(encrypted_message);
    fclose(priv_key);
    return 0;
}
