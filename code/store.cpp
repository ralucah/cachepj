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
    unsigned char *data;

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

    data = (unsigned char *)calloc(new_fsize, sizeof(unsigned char));
    if (!data) {
        cout << "Error allocating memory for data buffer" << endl;
        exit(1);
    }
    fread(data, new_fsize, 1, f);
    fclose(f);
    free(data);
    //cout << data << endl;

    // split data into chunks, add padding to last chunk if necessary
    const unsigned char *data_ptrs[k];
    for (int i = 0; i < k; i++) {
        data_ptrs[i] = data + i * bytes;
    }

    unsigned char *recovery_blocks = (unsigned char *)malloc(bytes * m);

    assert(!cauchy_256_encode(k, m, data_ptrs, recovery_blocks, bytes));

    Block *blocks = new Block[k];
    for (int i = 0; i < k; ++i) {
        blocks[i].data = (unsigned char*)data_ptrs[i];
        blocks[i].row = (unsigned char)i;
    }
    int i = 1;
    blocks[i].data = recovery_blocks + i * bytes;

    cout << "Before decode:" << endl;
    for (int i = 0; i < k; ++i) {
        cout << blocks[i].data << endl;
    }

    assert(!cauchy_256_decode(k, m, blocks, bytes));

    cout << "After decode:" << endl;
    for (int i = 0; i < k; ++i) {
        cout << blocks[i].data << endl;
    }

    return 0;
}
