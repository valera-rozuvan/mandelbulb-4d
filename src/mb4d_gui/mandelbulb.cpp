#include <math.h>
#include "mandelbulb.hpp"
#include "vec3_math.hpp"

const int MaximumRaySteps = 100;
const double MinimumDistance = 0.01;
// const int Iterations = 100;
// const double Scale = 10.2;
const double R = 3.0;

double max_float(float a, float b)
{
  if (a > b) {
    return a;
  }

  return b;
}

double DistanceEstimator(vec3_t point)
{
  return max_float(0.0, vec3_length(point) - R);
}

/*
double DistanceEstimator(vec3_t z)
{
  // double r;
  int n = 0;
  double temp = 0.0;

  while (n < Iterations) {
    if (z[0] + z[1] < 0) {
      // z.xy = -z.yx; // fold 1
      temp = z[0];
      z[0] = -1.0 * z[1];
      z[1] = -1.0 * temp;
    }
    if (z[0] + z[2] < 0) {
      // z.xz = -z.zx; // fold 2
      temp = z[0];
      z[0] = -1.0 * z[2];
      z[2] = -1.0 * temp;
    }
    if (z[1] + z[2] < 0) {
      // z.zy = -z.yz; // fold 3
      temp = z[2];
      z[2] = -1.0 * z[1];
      z[1] = -1.0 * temp;
    }

    // z = z*Scale - Offset*(Scale-1.0);
    // z = z*Scale;

    vec3_scale(z, Scale, NULL);

    n++;
  }

  return (vec3_length(z)) * pow(Scale, -(float)n);
}
*/

/*

float DE(vec3 z)
{
  z.xy = mod((z.xy),1.0)-vec3(0.5); // instance on xy-plane
  return length(z)-0.3;             // sphere DE
}



DE(p) = max(0.0, length(p)-R)  // solid sphere, zero interior



float DE(vec3 z)
{
  vec3 a1 = vec3(1,1,1);
  vec3 a2 = vec3(-1,-1,1);
  vec3 a3 = vec3(1,-1,-1);
  vec3 a4 = vec3(-1,1,-1);
  vec3 c;

  int n = 0;
  float dist, d;

  while (n < Iterations) {
    c = a1; dist = length(z-a1);

    d = length(z-a2); if (d < dist) { c = a2; dist=d; }
    d = length(z-a3); if (d < dist) { c = a3; dist=d; }
    d = length(z-a4); if (d < dist) { c = a4; dist=d; }

    z = Scale*z-c*(Scale-1.0);

    n++;
  }

  return length(z) * pow(Scale, float(-n));
}

*/

double trace(vec3_t from, vec3_t direction)
{
  double totalDistance = 0.0;
  int steps = 0;

  vec3_t point = vec3_create(NULL);
  vec3_t scaledDir = vec3_create(NULL);

  for (steps = 0; steps < MaximumRaySteps; steps += 1) {
    vec3_set(from, point);
    vec3_scale(direction, totalDistance, scaledDir);
    vec3_add(point, scaledDir, NULL);

    double distance = DistanceEstimator(point);
    totalDistance += distance;

    if (distance < MinimumDistance) break;
  }

  vec3_delete(point);
  vec3_delete(scaledDir);

  return 1.0 - (double)steps/(double)MaximumRaySteps;
}

void generateFractal(int layer, unsigned char *arrayMandel, const unsigned int wMandel, const unsigned int hMandel) {
  float m = 0;
  unsigned int i = 0;

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  vec3_t point = vec3_create(NULL);
  vec3_t direction = vec3_create(NULL);

  direction[0] = 0.0;
  direction[1] = 0.0;
  direction[2] = (double)layer * (1.0);

  for (unsigned int yy = 0; yy < hMandel; ++yy) {
    for (unsigned int xx = 0; xx < wMandel; ++xx) {
      x = -10.0 + (20.0 / (double)wMandel) * xx;
      y = -10.0 + (20.0 / (double)hMandel) * yy;
      z = (double)layer * (-10.0);

      point[0] = x;
      point[1] = y;
      point[2] = z;

      m = trace(point, direction);

      arrayMandel[i] = (unsigned int)(m * 255);
      arrayMandel[i + 1] = (unsigned int)(m * 255);
      arrayMandel[i + 2] = (unsigned int)(m * 255);
      arrayMandel[i + 3] = 255;

      i += 4;
    }
  }

  vec3_delete(point);
  vec3_delete(direction);
}

unsigned int mandel(double x0, double y0, double z0)
{
  const unsigned int n = 8;

  double nD = 0.0;

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  double r = 0.0;
  double theta = 0.0;
  double phi = 0.0;

  unsigned int i = 0;

  for (i = 0; i <= 32; i += 1) {
    nD = (double)n;

    r = sqrt(x * x + y * y + z * z);
    theta = atan2(sqrt(x * x + y * y), z);
    phi = atan2(y, x);

    x = pow(r, nD) * sin(theta * nD) * cos(phi * nD) + x0;
    y = pow(r, nD) * sin(theta * nD) * sin(phi * nD) + y0;
    z = pow(r, nD) * cos(theta * nD)                 + z0;

    if (pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0) > 2.0) {
      return 256 - (i * 4);
    }
  }

  return 0;
}

void generateMandel(int layer, unsigned char *arrayMandel, const unsigned int wMandel, const unsigned int hMandel) {
  unsigned int m = 0;
  unsigned int i = 0;

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  for (unsigned int yy = 0; yy < hMandel; ++yy) {
    for (unsigned int xx = 0; xx < wMandel; ++xx) {
      x = 4.0 * ((double)xx - ((double)wMandel / 2.0)) / (double)wMandel;
      y = 4.0 * ((double)yy - ((double)hMandel / 2.0)) / (double)hMandel;
      z = (double)layer / 50.0;

      m = mandel(x, y, z);

      arrayMandel[i] = m;
      arrayMandel[i + 1] = m;
      arrayMandel[i + 2] = m;
      arrayMandel[i + 3] = 255;

      i += 4;
    }
  }
}
