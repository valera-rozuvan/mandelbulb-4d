#include <cstddef>
#include <stdio.h>
#include <math.h>
#include "mcamera.hpp"
#include "utils.hpp"
#include "mandelbulb.hpp"

const unsigned int MaximumRaySteps = 320;
const double MinimumDistance = 0.001;
const double HalfMinimumDistance = 0.0005;

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

double DistanceEstimator(double xx, double yy, double zz)
{
  unsigned int i = 0;

  double zX = xx;
  double zY = yy;
  double zZ = zz;

  double theta = 0.0;
  double phi = 0.0;
  double zr = 0.0;

  double dr = 1.0;
  double r = 0.0;

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
    zX = zr * sin(theta) * cos(phi) + xx;
    zY = zr * sin(phi) * sin(theta) + yy;
    zZ = zr * cos(theta) + zz;
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
 * get_normal(P, N) - calculates normal vector N at point P
 *
 * Px, Py, Pz - coordinates of point P
 * Nx, Ny, Nz - components of vector N
 *
 */
void get_normal(double Px, double Py, double Pz, double* Nx, double* Ny, double* Nz)
{
  *Nx = RepeatedDistanceEstimator(Px + HalfMinimumDistance, Py, Pz) - RepeatedDistanceEstimator(Px - HalfMinimumDistance, Py, Pz);
  *Ny = RepeatedDistanceEstimator(Px, Py + HalfMinimumDistance, Pz) - RepeatedDistanceEstimator(Px, Py - HalfMinimumDistance, Pz);
  *Nz = RepeatedDistanceEstimator(Px, Py, Pz + HalfMinimumDistance) - RepeatedDistanceEstimator(Px, Py, Pz - HalfMinimumDistance);

  normalize(&(*Nx), &(*Ny), &(*Nz));
}

/*
 *
 * raymarch(P, D) - ray marching from point P in direction D
 *
 * Px, Py, Pz - coordinates of point P
 * Dx, Dy, Dz - components of vector N
 *
 * Returns distance to found point; if nothing is found, returns -1.
 *
 */
double raymarch(double Px, double Py, double Pz, double Dx, double Dy, double Dz)
{
  double totalDistance = 0.0;
  double distance;

  unsigned int steps;

  double Px_;
  double Py_;
  double Pz_;

  for (steps = 0; steps < MaximumRaySteps; steps += 1) {
    Px_ = Px + totalDistance * Dx;
    Py_ = Py + totalDistance * Dy;
    Pz_ = Pz + totalDistance * Dz;

    distance = RepeatedDistanceEstimator(Px_, Py_, Pz_, &DistanceEstimator);
    totalDistance += distance;

    if (distance < MinimumDistance) {
      return totalDistance;
    }
  }

  return -1.0;
}

void generateFractal(AppState* appState)
{
  unsigned int i = 0;

  double totalDistance;

  unsigned int x;
  unsigned int y;

  double temp_coef;

  double temp_clr_R, temp_clr_G, temp_clr_B;

  double cam_Px, cam_Py, cam_Pz;

  double Px, Py, Pz;
  double Dx, Dy, Dz;
  double Nx, Ny, Nz;

  appState->camera->get_P(&cam_Px, &cam_Py, &cam_Pz);
  appState->camera->cache__get_3d_point__constants(&(appState->wMandel), &(appState->hMandel));

  for (y = 0; y < appState->hMandel; y += 1) {
    // printf("y = %d (of %d)\n", y, appState->hMandel);

    for (x = 0; x < appState->wMandel; x += 1) {
      // Calculate 3D point in image plane.
      appState->camera->get_3d_point(
        &x, &y,
        &Px, &Py, &Pz
      );


      // Calculate direction for ray marching.
      Dx = Px - cam_Px;
      Dy = Py - cam_Py;
      Dz = Pz - cam_Pz;

      normalize(&Dx, &Dy, &Dz);


      // Ray marching.
      totalDistance = raymarch(Px, Py, Pz, Dx, Dy, Dz);


      if (totalDistance == -1) {
        appState->arrayMandel[i] = 0;
        appState->arrayMandel[i + 1] = 0;
        appState->arrayMandel[i + 2] = 0;
        appState->arrayMandel[i + 3] = 255;
      } else {
        // Calculate 3D point of fractal.
        Px += totalDistance * Dx;
        Py += totalDistance * Dy;
        Pz += totalDistance * Dz;


        // Get normal vector for 3D point of fractal.
        get_normal(Px, Py, Pz, &Nx, &Ny, &Nz);


        // Get a color for the 3D point.
        temp_coef = max_double(0.0, Nx * appState->LightSrc_x + Ny * appState->LightSrc_y + Nz * appState->LightSrc_z);

        temp_clr_R = appState->clr_R_ambient + appState->clr_R_diffuse * temp_coef;
        temp_clr_G = appState->clr_G_ambient + appState->clr_G_diffuse * temp_coef;
        temp_clr_B = appState->clr_B_ambient + appState->clr_B_diffuse * temp_coef;

        temp_coef = 20.0 * MinimumDistance * totalDistance;

        temp_clr_R = temp_clr_R / (temp_coef + temp_clr_R);
        temp_clr_G = temp_clr_G / (temp_coef + temp_clr_G);
        temp_clr_B = temp_clr_B / (temp_coef + temp_clr_B);

        temp_coef = 1.0 / (totalDistance - 1.0);

        temp_clr_R = temp_coef * (pow(totalDistance, temp_clr_R) - 1.0);
        temp_clr_G = temp_coef * (pow(totalDistance, temp_clr_G) - 1.0);
        temp_clr_B = temp_coef * (pow(totalDistance, temp_clr_B) - 1.0);

        if (temp_clr_R < 0) {
          temp_clr_R = 0;
        } else if (temp_clr_R > 1.0) {
          temp_clr_R = 1.0;
        }
        if (temp_clr_G < 0) {
          temp_clr_G = 0;
        } else if (temp_clr_G > 1.0) {
          temp_clr_G = 1.0;
        }
        if (temp_clr_B < 0) {
          temp_clr_B = 0;
        } else if (temp_clr_B > 1.0) {
          temp_clr_B = 1.0;
        }


        // Set the image pixel color.
        appState->arrayMandel[i] = (unsigned int)(255.0 * temp_clr_R);
        appState->arrayMandel[i + 1] = (unsigned int)(255.0 * temp_clr_G);
        appState->arrayMandel[i + 2] = (unsigned int)(255.0 * temp_clr_B);
        appState->arrayMandel[i + 3] = 255;
      }

      i += 4;
    }
  }
}
