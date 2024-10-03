#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

uint64_t A0 = 0x6a09e667f3bcc908;
uint64_t A1 = 0xbb67ae8584caa73b;
uint64_t A2 = 0x3c6ef372fe94f82b;
uint64_t A3 = 0xa54ff53a5f1d36f1;

#define B_SIZE 64
#define C_PREFIX "<BEGIN_COBRA_HASH>"
#define C_SUFFIX "<END_COBRA_HASH>"

void D(uint8_t *E, uint64_t *A0, uint64_t *A1, uint64_t *A2, uint64_t *A3) {
    for (int F = 0; F < B_SIZE; F++) {
        *A0 = (*A0 + E[F]) ^ (*A1);
        *A1 = (*A1 + E[F]) ^ (*A2);
        *A2 = (*A2 + E[F]) ^ (*A3);
        *A3 = (*A3 + E[F]) ^ (*A0);
        *A0 = (*A0 >> 3) | (*A0 << 61);
        *A1 = (*A1 >> 5) | (*A1 << 59);
        *A2 = (*A2 >> 7) | (*A2 << 57);
        *A3 = (*A3 >> 11) | (*A3 << 53);
    }
}

void cobrahash(FILE *H, unsigned char *X, unsigned char *Y) {
    uint8_t T[B_SIZE];
    size_t J;
    uint64_t A0_temp = A0;
    uint64_t A1_temp = A1;
    uint64_t A2_temp = A2;
    uint64_t A3_temp = A3;
    int K = 0;

    while ((J = fread(T, 1, B_SIZE, H)) > 0) {
        D(T, &A0_temp, &A1_temp, &A2_temp, &A3_temp);
    }
    
    sprintf((char *)X, "%016llx%016llx%016llx%016llx", A0_temp, A1_temp, A2_temp, A3_temp);
    
    rewind(H);
    unsigned char buffer[1];
    int byteCounter = 0;
    while ((J = fread(buffer, 1, 1, H)) != 0 && K < 32) {
        if (byteCounter % 6 == 0) {
            Y[K++] = buffer[0];
        }
        byteCounter++;
    }
    while (K < 32) {
        Y[K++] = 0x00;
    }
}

void N(unsigned char *O, int P) {
    for (int Q = 0; Q < P; Q++) {
        printf("%02x", O[Q]);
    }
}

void U() {
    printf("\033[1;31m");
    printf("   _____      _               _    _           _     \n");
    printf("  / ____|    | |     v0.3    | |  | |         | |    \n");
    printf(" | |     ___ | |__  _ __ __ _| |__| | __ _ ___| |__  \n");
    printf(" | |    / _ \\| '_ \\| '__/ _` |  __  |/ _` / __| '_ \\ \n");
    printf(" | |___| (_) | |_) | | | (_| | |  | | (_| \\__ \\ | | |\n");
    printf("  \\_____\\___/|_.__/|_|  \\__,_|_|  |_|\\__,_|___/_| |_|\n");
    printf("\033[0m\n");
}

void V() {
    printf("\033[1;32mUsage: cobra_hash [options] <filename>\n\033[0m");
    printf("Options:\n");
    printf("  \033[1;34m-h\033[0m             Show this help message\n");
    printf("  \033[1;34m-c <hash>\033[0m      Check if the hash of the file matches the provided hash in the format:\n");
    printf("                      %s<HASH>%s\n", C_PREFIX, C_SUFFIX);
    printf("\n\033[1;33mMade by JRDP Team\033[0m\n");
    printf("Contact: \033[1;34mhttps://jrdpteam.netlify.app\033[0m\n");
    printf("Main CobraHash page: \033[1;34mhttp://cobrahash.unaux.com\033[0m\n");
}

bool Z(const char *input_hash, FILE *H) {
    unsigned char X[65];
    unsigned char Y[32];
    char computed_hash[65 + 5 + 32 * 2 + 1];

    if (strncmp(input_hash, C_PREFIX, strlen(C_PREFIX)) != 0 || 
        strncmp(&input_hash[strlen(input_hash) - strlen(C_SUFFIX)], C_SUFFIX, strlen(C_SUFFIX)) != 0) {
        printf("\033[1;31mError: Hash must start with '%s' and end with '%s'\033[0m\n", C_PREFIX, C_SUFFIX);
        return false;
    }

    const char *actual_input_hash = input_hash + strlen(C_PREFIX);
    size_t actual_hash_length = strlen(actual_input_hash) - strlen(C_SUFFIX);

    if (actual_hash_length != (64 + 5 + 64)) {
        printf("\033[1;31mError: Invalid hash format length.\033[0m\n");
        return false;
    }

    rewind(H);
    cobrahash(H, X, Y);

    int pos = 0;
    pos += sprintf(&computed_hash[pos], "%s", X);
    pos += sprintf(&computed_hash[pos], "_000_");
    for (int A = 0; A < 32; A++) {
        pos += sprintf(&computed_hash[pos], "%02x", Y[A]);
    }

    if (strncmp(actual_input_hash, computed_hash, actual_hash_length) == 0) {
        return true;
    } else {
        return false;
    }
}

int main(int argc, char **argv) {
    U();

    if (argc < 2) {
        V();
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0) {
        V();
        return 0;
    }

    if (argc == 4 && strcmp(argv[1], "-c") == 0) {
        const char *provided_hash = argv[2];
        FILE *H = fopen(argv[3], "rb");
        if (!H) {
            perror("Error opening file");
            return 1;
        }

        if (Z(provided_hash, H)) {
            printf("\033[1;32mHash matches!\033[0m\n");
        } else {
            printf("\033[1;31mHash does not match!\033[0m\n");
        }

        fclose(H);
        return 0;
    }

    FILE *H = fopen(argv[1], "rb");
    if (!H) {
        perror("Error opening file");
        return 1;
    }

    unsigned char hash[65], bytes[32];
    cobrahash(H, hash, bytes);
    printf("%s", C_PREFIX);
    printf("%s_000_", hash);
    N(bytes, 32);
    printf("%s\n", C_SUFFIX);
    
    fclose(H);
    return 0;
}
