#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

uint64_t H0 = 0x6a09e667f3bcc908;
uint64_t H1 = 0xbb67ae8584caa73b;
uint64_t H2 = 0x3c6ef372fe94f82b;
uint64_t H3 = 0xa54ff53a5f1d36f1;

#define BLOCK_SIZE 64
#define HASH_PREFIX "<BEGIN_COBRA_HASH>"
#define HASH_SUFFIX "<END_COBRA_HASH>"

void process_block(uint8_t *block, uint64_t *H0, uint64_t *H1, uint64_t *H2, uint64_t *H3) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        *H0 = (*H0 + block[i]) ^ (*H1);
        *H1 = (*H1 + block[i]) ^ (*H2);
        *H2 = (*H2 + block[i]) ^ (*H3);
        *H3 = (*H3 + block[i]) ^ (*H0);

        *H0 = (*H0 >> 3) | (*H0 << 61);
        *H1 = (*H1 >> 5) | (*H1 << 59);
        *H2 = (*H2 >> 7) | (*H2 << 57);
        *H3 = (*H3 >> 11) | (*H3 << 53);
    }
}

void extract_32_bytes(FILE *file, unsigned char *outputBuffer) {
    size_t bytesRead;
    int index = 0;
    unsigned char buffer[1];
    int byteCounter = 0;

    while ((bytesRead = fread(buffer, 1, 1, file)) != 0 && index < 32) {
        if (byteCounter % 6 == 0) {
            outputBuffer[index++] = buffer[0];
        }
        byteCounter++;
    }

    while (index < 32) {
        outputBuffer[index++] = 0x00;
    }
}

void print_hex(unsigned char *hash, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02x", hash[i]);
    }
}

void cobra_custom_hash(FILE *file, unsigned char *outputBuffer) {
    uint8_t block[BLOCK_SIZE];
    size_t bytesRead;

    uint64_t H0_temp = H0;
    uint64_t H1_temp = H1;
    uint64_t H2_temp = H2;
    uint64_t H3_temp = H3;

    while ((bytesRead = fread(block, 1, BLOCK_SIZE, file)) > 0) {
        process_block(block, &H0_temp, &H1_temp, &H2_temp, &H3_temp);
    }

    sprintf((char *)outputBuffer, "%016llx%016llx%016llx%016llx", H0_temp, H1_temp, H2_temp, H3_temp);
}

void print_ascii_art() {
    printf("\033[1;31m");
    printf("   _____      _               _    _           _     \n");
    printf("  / ____|    | |             | |  | |         | |    \n");
    printf(" | |     ___ | |__  _ __ __ _| |__| | __ _ ___| |__  \n");
    printf(" | |    / _ \\| '_ \\| '__/ _` |  __  |/ _` / __| '_ \\ \n");
    printf(" | |___| (_) | |_) | | | (_| | |  | | (_| \\__ \\ | | |\n");
    printf("  \\_____\\___/|_.__/|_|  \\__,_|_|  |_|\\__,_|___/_| |_|\n");
    printf("\033[0m\n");
}

void print_help() {
    printf("\033[1;32mUsage: cobra_hash [options] <filename>\n\033[0m");
    printf("Options:\n");
    printf("  \033[1;34m-h\033[0m             Show this help message\n");
    printf("  \033[1;34m-c <hash>\033[0m      Check if the hash of the file matches the provided hash in the format:\n");
    printf("                      %s<YOUR_HASH>_000_<YOUR_32_BYTES>%s\n", HASH_PREFIX, HASH_SUFFIX);
    printf("\n\033[1;33mMade by JRDP Team\033[0m\n");
    printf("Contact: \033[1;34mhttps://jrdpteam.netlify.app\033[0m\n");
    printf("Main CobraHash page: \033[1;34mhttp://cobrahash.unaux.com\033[0m\n");
}

void cobra_hash(FILE *file) {
    unsigned char custom_hash[65];
    unsigned char extracted_bytes[32];

    rewind(file);
    cobra_custom_hash(file, custom_hash);

    rewind(file);
    extract_32_bytes(file, extracted_bytes);

    printf("%s", HASH_PREFIX);
    printf("%s_000_", custom_hash);
    print_hex(extracted_bytes, 32);
    printf("%s\n", HASH_SUFFIX);
}

bool check_hash(const char *input_hash, FILE *file) {
    unsigned char custom_hash[65];
    unsigned char extracted_bytes[32];
    char computed_hash[65 + 5 + 32 * 2 + 1];

    if (strncmp(input_hash, HASH_PREFIX, strlen(HASH_PREFIX)) != 0 || 
        strncmp(&input_hash[strlen(input_hash) - strlen(HASH_SUFFIX)], HASH_SUFFIX, strlen(HASH_SUFFIX)) != 0) {
        printf("\033[1;31mError: Hash must start with '%s' and end with '%s'\033[0m\n", HASH_PREFIX, HASH_SUFFIX);
        return false;
    }

    rewind(file);
    cobra_custom_hash(file, custom_hash);
    rewind(file);
    extract_32_bytes(file, extracted_bytes);

    int pos = 0;
    pos += sprintf(&computed_hash[pos], "%s", custom_hash);
    pos += sprintf(&computed_hash[pos], "_000_");
    for (int i = 0; i < 32; i++) {
        pos += sprintf(&computed_hash[pos], "%02x", extracted_bytes[i]);
    }

    return strcmp(input_hash, computed_hash) == 0;
}

int main(int argc, char **argv) {
    print_ascii_art();
    if (argc < 2) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    if (argc == 4 && strcmp(argv[1], "-c") == 0) {
        const char *provided_hash = argv[2];
        FILE *file = fopen(argv[3], "rb");
        if (!file) {
            perror("Error opening file");
            return 1;
        }

        if (check_hash(provided_hash, file)) {
            printf("\033[1;32mHash matches!\033[0m\n");
        } else {
            printf("\033[1;31mHash does not match!\033[0m\n");
        }

        fclose(file);
        return 0;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    cobra_hash(file);
    fclose(file);

    return 0;
}
