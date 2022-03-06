#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include <sys/mman.h>

#define BGRN "\e[1;32m"
#define BRED "\e[1;31m"
#define RESET_COL "\e[0m"

uint8_t hex_to_int(char c) {
    if (c >= '0' && c < ';') {return c - '0';}
    if (c >= 'A' && c < 'G') {return 10 + c - 'A';}
    if (c >= 'a' && c < 'g') {return 10 + c - 'a';}

    return 0;
}

void pattern_scan(const char *filename, char *hex_string) {
    char *buf;
    uint8_t *pattern_buf;    
    uint32_t pattern_len = 1, results_found;  
    uint64_t pattern_mask = UINT64_MAX;
    FILE *fp = fopen(filename, "rb");
    
    if (!fp) {
        printf("Failed to open file");
        return EXIT_FAILURE;
    }

    for (uint32_t i = 0; hex_string[i] != '\0'; ++i) {
        if (hex_string[i] == ' ') {
            pattern_len++;
        }
    }

    pattern_buf = malloc(pattern_len * sizeof(uint8_t));
    memset(pattern_buf, 0, pattern_len * sizeof(uint8_t));

    fseek(fp, 0, SEEK_END); 

    long len = ftell(fp);
    buf = malloc(len);

    fseek(fp, 0, SEEK_SET);    
    fread(buf, 1, len, fp);
    fclose(fp);

    printf("file size: 0x%LX bytes\n", (long long unsigned int)len);

    // scan for wildcards, defined as "??"
    for (uint32_t i = 0; i < pattern_len; ++i) {
        char *byte_str = &hex_string[i * 3];
        bool is_wildcard = byte_str[0] == '?';

        if (is_wildcard) {
            pattern_mask &= ~(1 << i);
        }
        else {
            pattern_buf[i] = 16 * hex_to_int(byte_str[0]);
            pattern_buf[i] += 1 * hex_to_int(byte_str[1]);
        }
    }

    // pattern scanning
    for (uintptr_t offset = 0; offset < len; ++offset) {
        const uint8_t *scan_buf = (uint8_t *) ((uintptr_t)buf + offset);
        bool pattern_found = true;
        // scan for wildcards
        for (uintptr_t i = 0; i < pattern_len; ++i) {
            if (!(pattern_mask &((uint64_t)1 << i)))
                continue;

            if (scan_buf[i] != pattern_buf[i]) {
                pattern_found = 0;
                break;
            }
        }
        if (pattern_found) {
            printf("pattern %d found at offset"BGRN" %p\n"RESET_COL, results_found, (void *)offset);
        }
    }

    free(buf); 
    free(pattern_buf);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Incorrect usage!\n");
        return EXIT_FAILURE;
    }
    if (argc > 3) {
        printf("Incorrect usage!\n");
        return EXIT_FAILURE;
    }
    pattern_scan(argv[1], argv[2]);
    return EXIT_SUCCESS;
}
