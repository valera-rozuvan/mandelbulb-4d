#include <math.h>
#include "boost/random.hpp"
#include <iostream>
#include <ctime>
#include "utils.hpp"

typedef boost::uniform_int<> NumberDistribution;
typedef boost::mt19937 RandomNumberGenerator;
typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

double max_double(double a, double b)
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

unsigned int rnd_from_range(const unsigned int rangeMin, const unsigned int rangeMax)
{
  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(std::time(0)); // seed with the current time

  return numberGenerator();
}
