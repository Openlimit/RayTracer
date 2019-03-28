//
// Created by wly on 2018/10/15.
//

#ifndef RAYTRACER_RAND48_H
#define RAYTRACER_RAND48_H

#include <stdlib.h>

#ifdef _WIN32

#define drand48_m 0x100000000LL
#define drand48_c 0xB16
#define drand48_a 0x5DEECE66DLL

static unsigned long long drand48_seed = 1;

float drand48(void)
{
    drand48_seed = (drand48_a * drand48_seed + drand48_c) & 0xFFFFFFFFFFFFLL;
    unsigned int x = drand48_seed >> 16;
    return 	((float)x / (float)drand48_m);

}

void srand48(unsigned int i)
{
    drand48_seed  = (((long long int)i) << 16) | rand();
}

#endif

#endif //RAYTRACER_RAND48_H
