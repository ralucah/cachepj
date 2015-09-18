#ifndef REEDSOL_H_
#define REEDSOL_H_

#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <gf_rand.h>
#include <unistd.h>
#include "jerasure.h"
#include "reed_sol.h"
#include "timing.h"

/* Function prototypes */
void rs_encode(char *file, int k, int m, int w, int packetsize, int buffersize);
void rs_decode(char *file);

#endif