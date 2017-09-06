#include <math.h>
#include "utils.hpp"

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
