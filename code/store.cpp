// g++ store.cpp liblonghair.a -I/home/ubuntu/cache-project/longhair/include -o store

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

#include "cauchy_256.h"

using namespace std;

void encode() {

}

int main(int argc, char* argv[]) {
    printf("First encode, then store!\n");

    if (cauchy_256_init()) {
        // Wrong static library
        exit(1);
    }

    if (argc != 4) {
        cout << "Usage: " << argv[0] << " inputfile k m" << endl;
        exit(1);
    }

    const char* filename = argv[1];
    const int k = atoi(argv[2]); // number of chunks to split data into
    const int m = atoi(argv[3]); // number of redundant chunks
    //const int bytes = 8 * 125;   // number of bytes per chunk; multiple of 8
    char *data;

    assert(k >= 0 && k < 256);
    assert(m >= 0 && m <= 256 - k);
    //assert(bytes % 8 == 0);

    FILE *f = fopen(filename, "rb");
    assert(f);
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);


    // is padding necessary?
    int new_fsize = fsize;
    if (fsize % (8 * k) != 0) {
        while (new_fsize % (8 * k) != 0)
            new_fsize++;
    }

    cout << "fsize: " << fsize << " new_fsize: " << new_fsize << endl;

    const int bytes = new_fsize / k; // number of bytes per chunk; multiple of 8
    assert (bytes % 8 == 0);
    cout << "bytes: " << bytes << endl;

    data = (char *)malloc(new_fsize + 1);
    if (!data) {
        cout << "Error allocating memory for data buffer" << endl;
        exit(1);
    }
    fread(data, new_fsize, 1, f);
    fclose(f);
    free(data);
    //cout << data << endl;

    // split data into chunks, add padding to last chunk if necessary
    return 0;

    /*unsigned char *data = new unsigned char[block_bytes * block_count];
    unsigned char *recovery_blocks = new unsigned char[block_bytes * recovery_block_count];
    Block *blocks = new Block[block_count];

    const unsigned char *data_ptrs[256];
    for (int i = 0; i < block_count; ++i) {
        data_ptrs[i] = data + i * block_bytes;
    }

    for (int i = 0; i < block_bytes * block_count; ++i) {
        data[i] = (unsigned char)'0';
    }

    assert(!cauchy_256_encode(block_count, recovery_block_count, data_ptrs, recovery_blocks, block_bytes));

    cout << "Before decode:" << endl;
    for (int i = 0; i < block_count; ++i) {
        cout << (int)blocks[i].row << endl;
    }

    assert(!cauchy_256_decode(block_count, recovery_block_count, blocks, block_bytes));

    cout << "After decode:" << endl;
    for (int i = 0; i < block_count; ++i) {
        cout << (int)blocks[i].row << endl;
    }*/

    //char *recoveryBlocks = new char[m * bytes];
    //char *dataPtrs[32];

    /* point data pointers to data here */
    //for (int i = 0; i < )

    /* encode the data */
    /*if (cauchy_256_encode(k, m, data_ptrs, recovery_blocks, bytes)) {
        // Input was invalid
        return false;
    }

    // For each recovery block,
    for (int ii = 0; ii < m; ++ii) {
        char *block = recovery_blocks + ii * bytes;
        unsigned char row = k + ii; // Transmit this with block (just one byte)

        // Transmit or store block bytes and row
    }

    delete []recovery_blocks;*/

    return 0;
}
