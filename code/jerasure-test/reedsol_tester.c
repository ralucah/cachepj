// gcc -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mavx -g -O3 -Wall -Wa,-q -lJerasure -ltiming -lgf_complete -o reedsol_tester reedsol.c reedsol_tester.c 

#include "reedsol.h"

int main (int argc, char **argv) {
    char *file;
    int k, m, w, packetsize;
    int buffersize;

    if (argc != 7) {
        fprintf(stderr,  "usage: inputfile k m w packetsize buffersize\n");
        fprintf(stderr,  "\n\nPacketsize is ignored for the reed_sol's");
        fprintf(stderr,  "\nBuffersize of 0 means the buffersize is chosen automatically.\n");
        fprintf(stderr,  "\nIf you just want to test speed, use an inputfile of \"-number\" where number is the size of the fake file you want to test.\n\n");
        exit(0);
    }

    file = (char*)malloc(sizeof(char)*(strlen(argv[1])));
    if (sscanf(argv[1], "%s", file) == 0) {
        fprintf(stderr,  "Could not read value for file\n");
        exit(0);
    } 
    if (sscanf(argv[2], "%d", &k) == 0) {
        fprintf(stderr,  "Could not read value for k\n");
        exit(0);
    }
    if (sscanf(argv[3], "%d", &m) == 0) {
        fprintf(stderr,  "Could not read value for m\n");
        exit(0);
    }
    if (sscanf(argv[4],"%d", &w) == 0) {
        fprintf(stderr,  "Could not read value for w.\n");
        exit(0);
    }
    if (sscanf(argv[5], "%d", &packetsize) == 0) {
            fprintf(stderr,  "Could not read value for packetsize.\n");
            exit(0);
    }
    if (sscanf(argv[6], "%d", &buffersize) == 0) {
            fprintf(stderr, "Could not read value for buffersize\n");
            exit(0);
    }

    /* Encode */
    printf("Encoding %s...\n", file);
    rs_encode(file, k, m, w, packetsize, buffersize);
    free(file);

    /* Decode */
    printf("Decoding %s...\n", file);
    rs_decode(file);

    return 0;
}