#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define YEL "\e[1;33m"
#define RED "\e[1;31m"
#define BLU "\e[1;34m"
#define GRN "\e[1;32m"
#define RST "\e[0;0m"
#define ERROR_MSG RED"[+] error: "RST

uint32_t results_found = -1;

uint8_t hex_char_to_int(char c) {
    if (c >= '0' && c < ';') {
        return c - '0';
    }
    if (c >= 'A' && c < 'G') {
        return 10 + c - 'A';
    }
    if (c >= 'a' && c < 'g') {
        return 10 + c - 'a';
    }

    return 0;
}

/*
 this function receives a directory path.
 returns 0 if a dir is found
 returns 1 if a file is found
 returns -1 if error
*/
int check_dir(const char* dirname) {
    DIR *dir = opendir(dirname);

    if(dir != NULL) {
        closedir(dir);
        return 0;
    }
    if(errno == ENOTDIR) {
        return 1;
    }
    
    return -1;
}

/*
 this function receives a file path and a hex string (pattern), witch it will iterate thru
 until the given hex string is found
*/
void pattern_scan(const char *filename, char *pattern) {
    char *buf;
    uint8_t *pattern_buf;    
    uint32_t pattern_len = 1;  
    uint64_t pattern_mask = UINT64_MAX;
    FILE *fp = fopen(filename, "rb");
    
    if (!fp) {
        fprintf(stderr, ERROR_MSG"pattern_scan() failed to open file\n");
        exit(1);
    }

    for (uint32_t i = 0; pattern[i] != '\0'; ++i) {
        if (pattern[i] == ' ') {
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

    printf(" file size: %ld bytes\n", (long long unsigned int)len);

    // scan for wildcards, defined as "??"
    for (uint32_t i = 0; i < pattern_len; ++i) {
        char *byte_str = &pattern[i * 3];
        bool is_wildcard = byte_str[0] == '?';

        if (is_wildcard) {
            pattern_mask &= ~(1 << i);
        }
        else {
            pattern_buf[i] = 16 * hex_char_to_int(byte_str[0]);
            pattern_buf[i] += 1 * hex_char_to_int(byte_str[1]);
        }
    }

    // pattern scanning
    for (uintptr_t offset = 0; offset < len; ++offset) {
        const uint8_t *scan_buf = (uint8_t *) ((uintptr_t)buf + offset);
        bool pattern_found = true;
        // process wildcards
        for (uintptr_t i = 0; i < pattern_len; ++i) {
            if (!(pattern_mask &((uint64_t)1 << i)))
                continue;

            if (scan_buf[i] != pattern_buf[i]) {
                pattern_found = 0;
                break;
            }
        }
        if (pattern_found) {
            results_found++;
            printf("pattern %d "GRN"found"RST" at offset"GRN" %p\n"RST, results_found, (void *)offset);
        }
    }
    if (results_found == (uint32_t)-1) {
        puts(RED"no pattern found"RST);
    }

    free(buf); 
    free(pattern_buf);
}

/*
 this function iterates thru an given directory and calls pattern_scan()
 for each file found, that is, every object found that is not a directory
*/
void seek_files(char *basePath, const int root, char *pattern) {
    char path[FILENAME_MAX];
    struct dirent *dir;
    DIR *directory = opendir(basePath);
    int64_t i = 0;

    if (!directory)
        return;

    while ((dir = readdir(directory)) != NULL) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dir->d_name);
            seek_files(path, root + 2, pattern);

            if (check_dir(path)) {
                printf("checking: "YEL"%s"RST, path);
                pattern_scan(path, pattern);
            }
        }
    }

    closedir(directory);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, ERROR_MSG"insufficient arguments usage!\n");
        return EXIT_FAILURE;
    }
    if (check_dir(argv[1]) == -1) {
        fprintf(stderr, ERROR_MSG"\"%s\" does not exist\n", argv[1]);
        exit(1);
    }
    seek_files(argv[1], 0, argv[2]);
    
    return 0;
}
