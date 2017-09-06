#include <cstddef>
#include <stdio.h>
#include <math.h>
#include "mcamera.hpp"
#include "utils.hpp"
#include "color.hpp"
#include "mandelbulb.hpp"

// const double R = 5.0;
//
// double DistanceEstimator(double x, double y, double z)
// {
//   double length = sqrt(x * x + y * y + z * z);
//
//   return max_double(0.0, length - R);
// }

const double bailout = 13.7;
const double Power = 42.08642;
const double fractalIters = 1000;

double DistanceEstimator(double Px, double Py, double Pz)
{
  double zX;
  double zY;
  double zZ;

  double dr;

  unsigned int i;

  double theta;
  double phi;
  double zr;

  double r;

  dr = 1.0;

  zX = Px;
  zY = Py;
  zZ = Pz;

  for (i = 0; i < fractalIters; i += 1) {
    r = sqrt(zX * zX + zY * zY + zZ * zZ);

    if (r > bailout) {
      break;
    }

    // Convert to polar coordinates.
    theta = acos(zZ / r);
    phi = atan2(zY, zX);
    dr =  pow(r, Power - 1.0) * Power * dr + 1.0;

    // Scale and rotate the point.
    zr = pow(r, Power);
    theta = theta * Power;
    phi = phi * Power;

    // Convert back to Cartesian coordinates.
    zX = zr * sin(theta) * cos(phi) + Px;
    zY = zr * sin(phi) * sin(theta) + Py;
    zZ = zr * cos(theta) + Pz;
  }

  return 0.5 * log(r) * r / dr;
}

double RepeatedDistanceEstimator(double Px, double Py, double Pz, double (*DE)(double, double, double))
{
  Px = mod(Px, 6.0) - 3.0;
  Py = mod(Py, 6.0) - 3.0;
  Pz = mod(Pz, 6.0) - 3.0;

  return (*DE)(Px, Py, Pz);
}

double RepeatedDistanceEstimator(double Px, double Py, double Pz)
{
  Px = mod(Px, 6.0) - 3.0;
  Py = mod(Py, 6.0) - 3.0;
  Pz = mod(Pz, 6.0) - 3.0;

  return DistanceEstimator(Px, Py, Pz);
}

/*
 *
 * get_normal(AppState, P, N) - calculates normal vector N at point P
 *
 * AppState - application state configuration
 * Px, Py, Pz - coordinates of point P
 * Nx, Ny, Nz - components of vector N
 *
 */
void get_normal(AppState* appState, double Px, double Py, double Pz, double* Nx, double* Ny, double* Nz)
{
  *Nx = RepeatedDistanceEstimator(Px + appState->HalfMinimumDistance, Py, Pz) -
        RepeatedDistanceEstimator(Px - appState->HalfMinimumDistance, Py, Pz);
  *Ny = RepeatedDistanceEstimator(Px, Py + appState->HalfMinimumDistance, Pz) -
        RepeatedDistanceEstimator(Px, Py - appState->HalfMinimumDistance, Pz);
  *Nz = RepeatedDistanceEstimator(Px, Py, Pz + appState->HalfMinimumDistance) -
        RepeatedDistanceEstimator(Px, Py, Pz - appState->HalfMinimumDistance);

  normalize(Nx, Ny, Nz);
}

/*
 *
 * raymarch(AppState, P, D) - ray marching from point P in direction D
 *
 * AppState - application state configuration
 * Px, Py, Pz - coordinates of point P
 * Dx, Dy, Dz - components of vector N
 *
 * Returns distance to found point; if nothing is found, returns -1.
 *
 */
double raymarch(AppState* appState, double Px, double Py, double Pz, double Dx, double Dy, double Dz)
{
  double totalDistance;
  double distance;

  unsigned int steps;

  double Px_;
  double Py_;
  double Pz_;

  totalDistance = 0.0;

  for (steps = 0; steps < appState->MaximumRaySteps; steps += 1) {
    Px_ = Px + totalDistance * Dx;
    Py_ = Py + totalDistance * Dy;
    Pz_ = Pz + totalDistance * Dz;

    distance = RepeatedDistanceEstimator(Px_, Py_, Pz_, &DistanceEstimator);
    totalDistance += distance;

    if (distance < appState->MinimumDistance) {
      return totalDistance;
    }
  }

  return -1.0;
}

void generateFractal(AppState* appState)
{
  unsigned int fractalIdx;

  double totalDistance;

  unsigned int x;
  unsigned int y;

  double cam_Px, cam_Py, cam_Pz;

  double Px, Py, Pz;
  double Dx, Dy, Dz;
  double Nx, Ny, Nz;

  appState->camera->get_P(&cam_Px, &cam_Py, &cam_Pz);
  appState->camera->cache__get_3d_point__constants(appState->wMandel, appState->hMandel);

  // Index for our 1D fractal image array.
  fractalIdx = 0;

  for (y = 0; y < appState->hMandel; y += 1) {
    // printf("y = %d (of %d)\n", y, appState->hMandel);

    for (x = 0; x < appState->wMandel; x += 1) {
      // Calculate 3D point in image plane.
      appState->camera->get_3d_point(x, y, &Px, &Py, &Pz);


      // Calculate direction for ray marching.
      Dx = Px - cam_Px;
      Dy = Py - cam_Py;
      Dz = Pz - cam_Pz;

      normalize(&Dx, &Dy, &Dz);


      // Ray marching.
      totalDistance = raymarch(appState, Px, Py, Pz, Dx, Dy, Dz);


      // Update normal vector for current point.
      if (totalDistance == -1) {
        Nx = 0.0;
        Ny = 0.0;
        Nz = 0.0;
      } else {
        // Calculate 3D point of fractal.
        Px += totalDistance * Dx;
        Py += totalDistance * Dy;
        Pz += totalDistance * Dz;


        // Get normal vector for 3D point of fractal.
        get_normal(appState, Px, Py, Pz, &Nx, &Ny, &Nz);
      }


      // Color our 2D pixel.
      simple_color_scheme2(appState, totalDistance, fractalIdx, Nx, Ny, Nz);


      // Increase fractal array index by 4 (because each 3D pixel is
      // represented by 4 color components; RGBA).
      fractalIdx += 4;
    }
  }
}
