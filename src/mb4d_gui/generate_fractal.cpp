#include <cstddef>
#include <stdio.h>
#include "mcamera.hpp"
#include "utils.hpp"
#include "color.hpp"
#include "distance_estimators.hpp"
#include "generate_fractal.hpp"

/*
 *
 * get_normal(AppState, P, N) - calculates normal vector N at point P
 *
 * AppState - application state configuration
 * Px, Py, Pz - coordinates of point P
 * Nx, Ny, Nz - components of vector N
 *
 */
void get_normal(
  AppState* appState,
  double Px, double Py, double Pz,
  double* Nx, double* Ny, double* Nz
)
{
  *Nx = (*(appState->DE))(appState, Px + appState->HalfMinimumDistance, Py, Pz) -
        (*(appState->DE))(appState, Px - appState->HalfMinimumDistance, Py, Pz);
  *Ny = (*(appState->DE))(appState, Px, Py + appState->HalfMinimumDistance, Pz) -
        (*(appState->DE))(appState, Px, Py - appState->HalfMinimumDistance, Pz);
  *Nz = (*(appState->DE))(appState, Px, Py, Pz + appState->HalfMinimumDistance) -
        (*(appState->DE))(appState, Px, Py, Pz - appState->HalfMinimumDistance);

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
double raymarch(
  AppState* appState,
  double Px, double Py, double Pz,
  double Dx, double Dy, double Dz
)
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

    distance = (*(appState->DE))(appState, Px_, Py_, Pz_);
    totalDistance += distance;

    if (distance < appState->MinimumDistance) {
      return totalDistance;
    }
  }

  return -1.0;
}

void generate_fractal(AppState* appState)
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


      // Increase fractal array index by 4 (because each 2D pixel is
      // represented by 4 color components; RGBA).
      fractalIdx += 4;
    }
  }
}
