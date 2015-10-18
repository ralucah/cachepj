// g++ store.cpp liblonghair.a -I/home/ubuntu/cache-project/longhair/include -g -o store
// gdb --args ./store encode wiki.txt 4 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

#include "cauchy_256.h"

using namespace std;

unsigned char* read_data_from_file(const char *filename, const unsigned int k, \
                              unsigned int *data_size, \
                              unsigned int *original_data_size) {
    unsigned char* data;
    long fsize, new_fsize;

    FILE *f = fopen(filename, "rb");
    assert(f);
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // is padding necessary?
    new_fsize = fsize;
    if (fsize % (8 * k) != 0) {
        while (new_fsize % (8 * k) != 0)
            new_fsize++;
    }

    cout << "fsize: " << fsize << " new_fsize: " << new_fsize << endl;

    data = (unsigned char *)calloc(new_fsize, sizeof(unsigned char));
    if (!data) {
        cout << "Error allocating memory for data buffer" << endl;
        exit(1);
    }
    fread(data, new_fsize, 1, f);
    fclose(f);
    //cout << data << endl;

    *data_size = new_fsize;
    *original_data_size = fsize;
    return data;
}

void write_blocks_to_file(const char *filename, const Block *blocks, \
                   const unsigned int num_blocks, \
                   const unsigned int block_size) {
    FILE *f_out;
    char filename_out[100];
    assert(strlen(filename) < 90);

    for (int i = 0; i < num_blocks; ++i) {
        sprintf(filename_out, "%s_%d", filename, i);
        //cout << "filename_out: " << filename_out << endl;
        f_out = fopen(filename_out, "wb");
        fwrite(blocks[i].data, block_size, 1, f_out);
        fclose(f_out);
    }
}

void free_blocks(Block *blocks, unsigned int num_blocks) {
    for (int i = 0; i < num_blocks; ++i) {
        free(blocks[i].data);
    }
}

Block* encode(const unsigned char *data, const unsigned int data_size, \
            const unsigned int k, const unsigned int m) {
    cout << "Encode data with k:" << k << " and m:" << m << endl;

    const int bytes_per_chunk = data_size / k; // number of bytes per chunk; multiple of 8
    assert (bytes_per_chunk % 8 == 0);
    cout << "bytes_per_chunk: " << bytes_per_chunk << endl;

    // split data into chunks, add padding to last chunk if necessary
    const unsigned char *data_ptrs[k];
    for (int i = 0; i < k; ++i) {
        data_ptrs[i] = data + i * bytes_per_chunk;
        //cout << i << "-------------------------------------------------------";
        //cout << data << endl;
    }

    unsigned char *recovery_blocks = (unsigned char *)malloc(bytes_per_chunk * m);

    assert(!cauchy_256_encode(k, m, data_ptrs, recovery_blocks, bytes_per_chunk));

    Block *blocks = new Block[k + m];

    for(int i = 0; i < k; ++i) {
        blocks[i].data = (unsigned char*)data_ptrs[i];
        blocks[i].row = (unsigned char)i;
    }

    for (int i = 0; i < m; ++i) {
        blocks[k + i].data = (unsigned char*)(recovery_blocks + i * bytes_per_chunk);
        blocks[k+ i].row = (unsigned char)i;
    }

    //free(recovery_blocks);
    return blocks;
}

Block* read_blocks(const char *filename, const unsigned int k, const unsigned int m) {
    Block *blocks = new Block[k];
    FILE *f;
    unsigned int fsize;
    char blockname[100];
    assert(strlen(filename) < 90);
    int block_num = 0;
    int i = 0;
    bool enough_blocks = false;
    while (i < (k + m) && enough_blocks == false) {
        sprintf(blockname, "%s_%d", filename, i);
        cout << "blockname: " << blockname << endl;
        f = fopen(blockname, "rb");
        if (f != NULL) {
            fseek(f, 0, SEEK_END);
            fsize = ftell(f);
            fseek(f, 0, SEEK_SET);
            cout << "fsize: " << fsize << endl;
            blocks[block_num].data = (unsigned char*)malloc(fsize * sizeof(unsigned char));
            fread(blocks[block_num].data, fsize, 1, f);
            blocks[block_num].row = (unsigned char)i;
            fclose(f);
            block_num++;
            if (block_num == k)
                enough_blocks = true;
        }
        i++;
    }

    return blocks;
}

Block* decode(const char *filename, Block *blocks, \
            const unsigned int k, const unsigned int m, \
            const unsigned int bytes) {
    //cout << "Decode " << filename << " with k:" << k << " and m:" << m << endl;
    for (int i = 0; i < k; ++i) {
        cout << (int)blocks[i].row << endl;
    }
    assert(!cauchy_256_decode(k, m, blocks, bytes));

    cout << "After decode:" << endl;
    for (int i = 0; i < k; ++i) {
        cout << (int)blocks[i].row << endl;
        //cout << blocks[i].data << endl;
    }

    return blocks;
}

void write_blocks_to_one_file(const char* filename, Block *blocks, const unsigned int k, \
                          const unsigned int block_size, \
                          const unsigned int original_data_size) {

    char filename_out[100];
    sprintf(filename_out, "%s_decoded", filename);
    FILE *f;
    f = fopen(filename_out, "wb");
    assert(f);
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            if (blocks[j].row == i) {
                cout << "Printing row " << (int)blocks[j].row << endl;
                if (i != k - 1) {
                    fwrite(blocks[j].data, block_size, 1, f);
                } else {
                    fwrite(blocks[j].data, block_size - (k * block_size - original_data_size), 1, f);
                }
                break;
            }
        }
    }
    fclose(f);
}

int main(int argc, char* argv[]) {
    printf("First encode, then store!\n");

    if (cauchy_256_init()) {
        // Wrong static library
        exit(1);
    }

    if (argc != 5) {
        cout << "Usage: " << argv[0] << " operation inputfile k m" << endl;
        exit(1);
    }

    const char *operation = argv[1];
    const char *filename = argv[2];
    const unsigned int k = atoi(argv[3]); // number of chunks to split data into
    const unsigned int m = atoi(argv[4]); // number of redundant chunks

    assert(k >= 0 && k < 256);
    assert(m >= 0 && m <= 256 - k);

    unsigned int data_size, original_data_size;
    unsigned char *data = read_data_from_file(filename, k, &data_size, &original_data_size);

    if (strcmp(operation, "encode") == 0) {
        Block *blocks = encode(data, data_size, k, m);
        //free(data);
        write_blocks_to_file(filename, blocks, k + m, data_size / k);
    } else if (strcmp(operation, "decode") == 0) {
        Block *blocks = read_blocks(filename, k, m);
        Block *decoded_blocks = decode(filename, blocks, k, m, data_size / k);
        write_blocks_to_one_file(filename, decoded_blocks, k, data_size / k, original_data_size);
    } else {
        cout << "Error: unknown operation" << endl;
        exit(1);
    }

    /*assert(!cauchy_256_decode(k, m, blocks, bytes));

    cout << "After decode:" << endl;
    for (int i = 0; i < k; ++i) {
        cout << (int)blocks[i].row << endl;
        //cout << blocks[i].data << endl;
    }
*/
    return 0;
}
