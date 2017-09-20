#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>

double maxDouble(double, double);

void normalize(double*, double*, double*);

double mod(double, double);

unsigned int rndFromRange(const unsigned int, const unsigned int);

uint64_t uint64UniformRandom(uint64_t, uint64_t);

#endif // UTILS_HPP
