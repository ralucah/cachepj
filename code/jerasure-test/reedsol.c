// gcc -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mavx -g -O3 -Wall -Wa,-q -lJerasure -ltiming -lgf_complete -o reedsol reedsol.c

/* *
 * Copyright (c) 2014, James S. Plank and Kevin Greenan
 * All rights reserved.
 *
 * Jerasure - A C/C++ Library for a Variety of Reed-Solomon and RAID-6 Erasure
 * Coding Techniques
 *
 * Revision 2.0: Galois Field backend now links to GF-Complete
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 *  - Neither the name of the University of Tennessee nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Jerasure's authors:

   Revision 2.x - 2014: James S. Plank and Kevin M. Greenan.
   Revision 1.2 - 2008: James S. Plank, Scott Simmerman and Catherine D. Schuman.
   Revision 1.0 - 2007: James S. Plank.
 */

/* 
This program takes as input an inputfile, k, m, a coding 
technique, w, and packetsize.  It creates k+m files from 
the original file so that k of these files are parts of 
the original file and m of the files are encoded based on 
the given coding technique. The format of the created files 
is the file name with "_k#" or "_m#" and then the extension.  
(For example, inputfile test.txt would yield file "test_k1.txt".)
*/

#include "reedsol.h"

char *coding_technique = "reed_sol_van";

int jfread(void *ptr, int size, int nmembers, FILE *stream)
{
  if (stream != NULL) return fread(ptr, size, nmembers, stream);

  MOA_Fill_Random_Region(ptr, size);
  return size;
}

void rs_encode(char *file, int k, int m, int w, int packetsize, int buffersize) {
    int readins, n;

    FILE *fp, *fp2;             // file pointers
    char *block;                // padding file
    int size, newsize;          // size of file and temp size 
    struct stat status;         // finding file size

    int i;                      // loop control variables
    int blocksize;              // size of k+m files
    int total;
    int extra;
    
    /* Jerasure Arguments */
    char **data;                
    char **coding;
    int *matrix;
    
    /* Creation of file name variables */
    char temp[5];
    char *s1, *s2, *extension;
    char *fname;
    int md;
    char *curdir;

    /* Timing variables */
    struct timing t1, t2, t3, t4;
    double tsec;
    double totalsec;
    struct timing start;

    /* Find buffersize */
    int up, down;

    /* Start timing */
    timing_set(&t1);
    totalsec = 0.0;
    matrix = NULL;

    /* Error check Arguments */
    if (k <= 0) {
        fprintf(stderr,  "Invalid value for k\n");
        exit(0);
    }
    if (m < 0) {
        fprintf(stderr,  "Invalid value for m\n");
        exit(0);
    }
    if (w != 8 && w != 16 && w != 32) {
        fprintf(stderr,  "w must be one of {8, 16, 32}\n");
        exit(0);
    }
    if (packetsize < 0) {
        fprintf(stderr,  "Invalid value for packetsize.\n");
        exit(0);
    }
    if (buffersize < 0) {
        fprintf(stderr, "Invalid value for buffersize\n");
        exit(0);
   }

    /* Determine proper buffersize by finding the closest valid buffersize to the input value  */
    if (buffersize != 0) {
        if (packetsize != 0 && buffersize%(sizeof(long)*w*k*packetsize) != 0) { 
            up = buffersize;
            down = buffersize;
            while (up%(sizeof(long)*w*k*packetsize) != 0 && (down%(sizeof(long)*w*k*packetsize) != 0)) {
                up++;
                if (down == 0) {
                    down--;
                }
            }
            if (up%(sizeof(long)*w*k*packetsize) == 0) {
                buffersize = up;
            }
            else {
                if (down != 0) {
                    buffersize = down;
                }
            }
        }
        else if (packetsize == 0 && buffersize%(sizeof(long)*w*k) != 0) {
            up = buffersize;
            down = buffersize;
            while (up%(sizeof(long)*w*k) != 0 && down%(sizeof(long)*w*k) != 0) {
                up++;
                down--;
            }
            if (up%(sizeof(long)*w*k) == 0) {
                buffersize = up;
            }
            else {
                buffersize = down;
            }
        }
    }

    /* Get current working directory for construction of file names */
    curdir = (char*)malloc(sizeof(char)*1000);  
    assert(curdir == getcwd(curdir, 1000));
    if (file[0] != '-') {

        /* Open file and error check */
        fp = fopen(file, "rb");
        if (fp == NULL) {
            fprintf(stderr,  "Unable to open file.\n");
            exit(0);
        }
    
        /* Create Coding directory */
        i = mkdir("Coding", S_IRWXU);
        if (i == -1 && errno != EEXIST) {
            fprintf(stderr, "Unable to create Coding directory.\n");
            exit(0);
        }
    
        /* Determine original size of file */
        stat(file, &status); 
        size = status.st_size;
        } else {
            if (sscanf(file+1, "%d", &size) != 1 || size <= 0) {
                    fprintf(stderr, "Files starting with '-' should be sizes for randomly created input\n");
                    exit(1);
            }
            fp = NULL;
            MOA_Seed(time(0));
        }

     newsize = size;
    
    /* Find new size by determining next closest multiple */
    if (packetsize != 0) {
        if (size%(k*w*packetsize*sizeof(long)) != 0) {
            while (newsize%(k*w*packetsize*sizeof(long)) != 0) 
                newsize++;
        }
    }
    else {
        if (size%(k*w*sizeof(long)) != 0) {
            while (newsize%(k*w*sizeof(long)) != 0) 
                newsize++;
        }
    }
    
    if (buffersize != 0) {
        while (newsize%buffersize != 0) {
            newsize++;
        }
    }

     /* Determine size of k+m files */
    blocksize = newsize/k;

    /* Allow for buffersize and determine number of read-ins */
    if (size > buffersize && buffersize != 0) {
        if (newsize%buffersize != 0) {
            readins = newsize/buffersize;
        }
        else {
            readins = newsize/buffersize;
        }
        block = (char *)malloc(sizeof(char)*buffersize);
        blocksize = buffersize/k;
    }
    else {
        readins = 1;
        buffersize = size;
        block = (char *)malloc(sizeof(char)*newsize);
    }

     /* Break inputfile name into the filename and extension */  
    s1 = (char*)malloc(sizeof(char)*(strlen(file)+20));
    s2 = strrchr(file, '/');
    if (s2 != NULL) {
        s2++;
        strcpy(s1, s2);
    }
    else {
        strcpy(s1, file);
    }
    s2 = strchr(s1, '.');
    if (s2 != NULL) {
          extension = strdup(s2);
          *s2 = '\0';
    } else {
          extension = strdup("");
    }

    /* Allocate for full file name */
    fname = (char*)malloc(sizeof(char)*(strlen(file)+strlen(curdir)+20));
    sprintf(temp, "%d", k);
    md = strlen(temp);
    
    /* Allocate data and coding */
    data = (char **)malloc(sizeof(char*)*k);
    coding = (char **)malloc(sizeof(char*)*m);
    for (i = 0; i < m; i++) {
        coding[i] = (char *)malloc(sizeof(char)*blocksize);
                if (coding[i] == NULL) { perror("malloc"); exit(1); }
    }

    /* Create coding matrix or bitmatrix and schedule */
    timing_set(&t3);
    matrix = reed_sol_vandermonde_coding_matrix(k, m, w);
    timing_set(&start);
    timing_set(&t4);
    totalsec += timing_delta(&t3, &t4);

    /* Read in data until finished */
    n = 1;
    total = 0;

    while (n <= readins) {
        /* Check if padding is needed, if so, add appropriate 
           number of zeros */
        if (total < size && total+buffersize <= size) {
            total += jfread(block, sizeof(char), buffersize, fp);
        }
        else if (total < size && total+buffersize > size) {
            extra = jfread(block, sizeof(char), buffersize, fp);
            for (i = extra; i < buffersize; i++) {
                block[i] = '0';
            }
        }
        else if (total == size) {
            for (i = 0; i < buffersize; i++) {
                block[i] = '0';
            }
        }
    
        /* Set pointers to point to file data */
        for (i = 0; i < k; i++) {
            data[i] = block+(i*blocksize);
        }

        timing_set(&t3);
        jerasure_matrix_encode(k, m, w, matrix, data, coding, blocksize);
        timing_set(&t4);
    
        /* Write data and encoded data to k+m files */
        for (i = 1; i <= k; i++) {
            if (fp == NULL) {
                bzero(data[i-1], blocksize);
            } else {
                sprintf(fname, "%s/Coding/%s_k%0*d%s", curdir, s1, md, i, extension);
                if (n == 1) {
                    fp2 = fopen(fname, "wb");
                }
                else {
                    fp2 = fopen(fname, "ab");
                }
                fwrite(data[i-1], sizeof(char), blocksize, fp2);
                fclose(fp2);
            }
            
        }
        for (i = 1; i <= m; i++) {
            if (fp == NULL) {
                bzero(data[i-1], blocksize);
            } else {
                sprintf(fname, "%s/Coding/%s_m%0*d%s", curdir, s1, md, i, extension);
                if (n == 1) {
                    fp2 = fopen(fname, "wb");
                }
                else {
                    fp2 = fopen(fname, "ab");
                }
                fwrite(coding[i-1], sizeof(char), blocksize, fp2);
                fclose(fp2);
            }
        }
        n++;
        /* Calculate encoding time */
        totalsec += timing_delta(&t3, &t4);
    }

    /* Create metadata file */
        if (fp != NULL) {
        sprintf(fname, "%s/Coding/%s_meta.txt", curdir, s1);
        fp2 = fopen(fname, "wb");
        fprintf(fp2, "%s\n", file);
        fprintf(fp2, "%d\n", size);
        fprintf(fp2, "%d %d %d %d %d\n", k, m, w, packetsize, buffersize);
        //fprintf(fp2, "%s\n", argv[4]);
        //fprintf(fp2, "%d\n", tech);
        fprintf(fp2, "%s\n", coding_technique);
        fprintf(fp2, "%d\n", readins);
        fclose(fp2);
    }

    /* Free allocated memory */
    free(s1);
    free(fname);
    free(block);
    free(curdir);
    
    /* Calculate rate in MB/sec and print */
    timing_set(&t2);
    tsec = timing_delta(&t1, &t2);
    printf("Encoding (MB/sec): %0.10f\n", (((double) size)/1024.0/1024.0)/totalsec);
    printf("En_Total (MB/sec): %0.10f\n", (((double) size)/1024.0/1024.0)/tsec);
}

void rs_decode(char *file) {
    int readins, n;

    FILE *fp;               // File pointer

    /* Jerasure arguments */
    char **data;
    char **coding;
    int *erasures;
    int *erased;
    int *matrix;

    /* Parameters */
    int k, m, w, packetsize, buffersize;

    int i, j;               // loop control variable, s
    int blocksize = 0;      // size of individual files
    int origsize;           // size of file before padding
    int total;              // used to write data, not padding to file
    struct stat status;     // used to find size of individual files
    int numerased;          // number of erased files
        
    /* Used to recreate file names */
    char *temp;
    char *cs1, *cs2, *extension;
    char *fname;
    int md;
    char *curdir;

    /* Used to time decoding */
    struct timing t1, t2, t3, t4;
    double tsec;
    double totalsec;

    matrix = NULL;
    totalsec = 0.0;
    
    /* Start timing */
    timing_set(&t1);

    curdir = (char *)malloc(sizeof(char)*1000);
    assert(curdir == getcwd(curdir, 1000));
    
    /* Begin recreation of file names */
    cs1 = (char*)malloc(sizeof(char)*strlen(file));
    cs2 = strrchr(file, '/');
    if (cs2 != NULL) {
        cs2++;
        strcpy(cs1, cs2);
    }
    else {
        strcpy(cs1, file);
    }
    cs2 = strchr(cs1, '.');
    if (cs2 != NULL) {
                extension = strdup(cs2);
        *cs2 = '\0';
    } else {
           extension = strdup("");
        }   
    fname = (char *)malloc(sizeof(char*)*(100+strlen(file)+20));

    /* Read in parameters from metadata file */
    sprintf(fname, "%s/Coding/%s_meta.txt", curdir, cs1);

    fp = fopen(fname, "rb");
    if (fp == NULL) {
      fprintf(stderr, "Error: no metadata file %s\n", fname);
      exit(1);
    }

    temp = (char *)malloc(sizeof(char)*(strlen(file)+20));
    if (fscanf(fp, "%s", temp) != 1) {
        fprintf(stderr, "Metadata file - bad format\n");
        exit(0);
    }
    if (fscanf(fp, "%d", &origsize) != 1) {
        fprintf(stderr, "Original size is not valid\n");
        exit(0);
    }
    if (fscanf(fp, "%d %d %d %d %d", &k, &m, &w, &packetsize, &buffersize) != 5) {
        fprintf(stderr, "Parameters are not correct\n");
        exit(0);
    }
    coding_technique = (char *)malloc(sizeof(char)*(strlen(file)+20));
    if (fscanf(fp, "%s", coding_technique) != 1) {
        fprintf(stderr, "Metadata file - bad format\n");
        exit(0);
    }
    if (fscanf(fp, "%d", &readins) != 1) {
        fprintf(stderr, "Metadata file - bad format\n");
        exit(0);
    }
    fclose(fp);

    /* Allocate memory */
    erased = (int *)malloc(sizeof(int)*(k+m));
    for (i = 0; i < k+m; i++)
        erased[i] = 0;
    erasures = (int *)malloc(sizeof(int)*(k+m));

    data = (char **)malloc(sizeof(char *)*k);
    coding = (char **)malloc(sizeof(char *)*m);
    if (buffersize != origsize) {
        for (i = 0; i < k; i++) {
            data[i] = (char *)malloc(sizeof(char)*(buffersize/k));
        }
        for (i = 0; i < m; i++) {
            coding[i] = (char *)malloc(sizeof(char)*(buffersize/k));
        }
        blocksize = buffersize/k;
    }

    sprintf(temp, "%d", k);
    md = strlen(temp);
    timing_set(&t3);

    /* Create coding matrix or bitmatrix */
    matrix = reed_sol_vandermonde_coding_matrix(k, m, w);
    timing_set(&t4);
    totalsec += timing_delta(&t3, &t4);
    
    /* Begin decoding process */
    total = 0;
    n = 1;  
    while (n <= readins) {
        numerased = 0;
        /* Open files, check for erasures, read in data/coding */   
        for (i = 1; i <= k; i++) {
            sprintf(fname, "%s/Coding/%s_k%0*d%s", curdir, cs1, md, i, extension);
            fp = fopen(fname, "rb");
            if (fp == NULL) {
                erased[i-1] = 1;
                erasures[numerased] = i-1;
                numerased++;
                //printf("%s failed\n", fname);
            }
            else {
                if (buffersize == origsize) {
                    stat(fname, &status);
                    blocksize = status.st_size;
                    data[i-1] = (char *)malloc(sizeof(char)*blocksize);
                    assert(blocksize == fread(data[i-1], sizeof(char), blocksize, fp));
                }
                else {
                    fseek(fp, blocksize*(n-1), SEEK_SET); 
                    assert(buffersize/k == fread(data[i-1], sizeof(char), buffersize/k, fp));
                }
                fclose(fp);
            }
        }
        for (i = 1; i <= m; i++) {
            sprintf(fname, "%s/Coding/%s_m%0*d%s", curdir, cs1, md, i, extension);
                fp = fopen(fname, "rb");
            if (fp == NULL) {
                erased[k+(i-1)] = 1;
                erasures[numerased] = k+i-1;
                numerased++;
                //printf("%s failed\n", fname);
            }
            else {
                if (buffersize == origsize) {
                    stat(fname, &status);
                    blocksize = status.st_size;
                    coding[i-1] = (char *)malloc(sizeof(char)*blocksize);
                    assert(blocksize == fread(coding[i-1], sizeof(char), blocksize, fp));
                }
                else {
                    fseek(fp, blocksize*(n-1), SEEK_SET);
                    assert(blocksize == fread(coding[i-1], sizeof(char), blocksize, fp));
                }   
                fclose(fp);
            }
        }
        /* Finish allocating data/coding if needed */
        if (n == 1) {
            for (i = 0; i < numerased; i++) {
                if (erasures[i] < k) {
                    data[erasures[i]] = (char *)malloc(sizeof(char)*blocksize);
                }
                else {
                    coding[erasures[i]-k] = (char *)malloc(sizeof(char)*blocksize);
                }
            }
        }
        
        erasures[numerased] = -1;
        timing_set(&t3);
    
        /* Choose proper decoding method */
        if (strcmp(coding_technique, "reed_sol_van") == 0) {
            i = jerasure_matrix_decode(k, m, w, matrix, 1, erasures, data, coding, blocksize);
        }
        else {
            fprintf(stderr, "Not a valid coding technique.\n");
            exit(0);
        }
        timing_set(&t4);
    
        /* Exit if decoding was unsuccessful */
        if (i == -1) {
            fprintf(stderr, "Unsuccessful!\n");
            exit(0);
        }
    
        /* Create decoded file */
        sprintf(fname, "%s/Coding/%s_decoded%s", curdir, cs1, extension);
        if (n == 1) {
            fp = fopen(fname, "wb");
        }
        else {
            fp = fopen(fname, "ab");
        }
        for (i = 0; i < k; i++) {
            if (total+blocksize <= origsize) {
                fwrite(data[i], sizeof(char), blocksize, fp);
                total+= blocksize;
            }
            else {
                for (j = 0; j < blocksize; j++) {
                    if (total < origsize) {
                        fprintf(fp, "%c", data[i][j]);
                        total++;
                    }
                    else {
                        break;
                    }
                    
                }
            }
        }
        n++;
        fclose(fp);
        totalsec += timing_delta(&t3, &t4);
    }
    
    /* Free allocated memory */
    free(cs1);
    free(extension);
    free(fname);
    free(data);
    free(coding);
    free(erasures);
    free(erased);

    /* Stop timing and print time */
    timing_set(&t2);
    tsec = timing_delta(&t1, &t2);
    printf("Decoding (MB/sec): %0.10f\n", (((double) origsize)/1024.0/1024.0)/totalsec);
    printf("De_Total (MB/sec): %0.10f\n\n", (((double) origsize)/1024.0/1024.0)/tsec);
}