#include <math.h>
#include "boost/random.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>

#if defined(_WIN32) || defined(WIN32)
#include "rand48.h"
#endif

#include "utils.hpp"

typedef boost::uniform_int<> NumberDistribution;
typedef boost::mt19937 RandomNumberGenerator;
typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

double maxDouble(double a, double b)
{
  if (a > b) {
    return a;
  }

  return b;
}

void normalize(double* x, double* y, double* z)
{
  double length = sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));

  *x /= length;
  *y /= length;
  *z /= length;
}

double mod(double a, double N)
{
  return a - N * floor(a / N);
}

unsigned int rndFromRange(const unsigned int rangeMin, const unsigned int rangeMax)
{
  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(std::time(0) + (rand() % 1000) + 1); // seed with the current time

  return numberGenerator();
}

// Taken from:
//   http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
bool isPowerOfTwo(uint64_t x)
{
  return ((x != 0) && ((x & (~x + 1)) == x));
}

// Taken from:
//   https://stackoverflow.com/a/20177601
uint64_t urand64(void)
{
  uint64_t hi = lrand48();
  uint64_t md = lrand48();
  uint64_t lo = lrand48();

  return (hi << 42) + (md << 21) + lo;
}

uint64_t uint64UniformRandom(uint64_t low, uint64_t high)
{
  static const uint64_t M = ~(uint64_t)0;

  uint64_t range = high - low;
  uint64_t toExclude = isPowerOfTwo(range) ? 0 : M % range + 1;
  uint64_t res;

  // Eliminate `toExclude` possible values from consideration.
  while ((res = urand64()) < toExclude) {}

  return low + res % range;
}
