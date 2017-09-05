#include <cstddef>
#include <stdio.h>
#include <math.h>
#include "mcamera.hpp"
#include "mandelbulb.hpp"
#include "vec3_math.hpp"

const int MaximumRaySteps = 320;
const double MinimumDistance = 0.001;
const double HalfMinimumDistance = 0.0005;

double max_double(double a, double b)
{
  if (a > b) {
    return a;
  }

  return b;
}

void normalize(double* x, double* y, double* z) {
  double length = sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));

  *x /= length;
  *y /= length;
  *z /= length;
}

void normalize(vec3_t* P) {
  double length = sqrt((*P)[0] * (*P)[0] + (*P)[1] * (*P)[1] + (*P)[2] * (*P)[2]);

  (*P)[0] /= length;
  (*P)[1] /= length;
  (*P)[2] /= length;
}

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

double DistanceEstimator(double xx, double yy, double zz) {
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

double mod(double a, double N) {
  return a - N * floor(a / N);
}

double RepeatedDistanceEstimator(double xx, double yy, double zz) {
    xx = mod(xx, 6.0) - 3.0;
    yy = mod(yy, 6.0) - 3.0;
    zz = mod(zz, 6.0) - 3.0;

    return DistanceEstimator(xx, yy, zz);
}

void get_normal(vec3_t* from, vec3_t* direction, double totalDistance, vec3_t* normal_v) {
  double Px = (*from)[0] + totalDistance * ((*direction)[0]);
  double Py = (*from)[1] + totalDistance * ((*direction)[1]);
  double Pz = (*from)[2] + totalDistance * ((*direction)[2]);

  (*normal_v)[0] = RepeatedDistanceEstimator(Px + HalfMinimumDistance, Py, Pz) - RepeatedDistanceEstimator(Px - HalfMinimumDistance, Py, Pz);
  (*normal_v)[1] = RepeatedDistanceEstimator(Px, Py + HalfMinimumDistance, Pz) - RepeatedDistanceEstimator(Px, Py - HalfMinimumDistance, Pz);
  (*normal_v)[2] = RepeatedDistanceEstimator(Px, Py, Pz + HalfMinimumDistance) - RepeatedDistanceEstimator(Px, Py, Pz - HalfMinimumDistance);

  normalize(&(*normal_v));
}

double raymarch(vec3_t* from, vec3_t* direction)
{
  double totalDistance = 0.0;
  double distance = 0.0;
  unsigned int steps = 0;

  double Px = 0.0;
  double Py = 0.0;
  double Pz = 0.0;

  for (steps = 0; steps < MaximumRaySteps; steps += 1) {
    Px = (*from)[0] + totalDistance * ((*direction)[0]);
    Py = (*from)[1] + totalDistance * ((*direction)[1]);
    Pz = (*from)[2] + totalDistance * ((*direction)[2]);

    distance = RepeatedDistanceEstimator(Px, Py, Pz);
    totalDistance += distance;

    if (distance < MinimumDistance) {
      return totalDistance;
    }
  }

  return -1.0;
}

void generateFractal(
  AppState* appState
) {
  double totalDistance = 0;

  unsigned int i = 0;
  unsigned int x = 0;
  unsigned int y = 0;

  double LightSrc_x = -23.0;
  double LightSrc_y = 17.0;
  double LightSrc_z = 12.0;

  double temp_coef = 0.0;

  normalize(&LightSrc_x, &LightSrc_y, &LightSrc_z);

  double clr_R_ambient = 0.019607843137255;
  double clr_G_ambient = 0.145098039215686;
  double clr_B_ambient = 0.376470588235294;

  double clr_R_diffuse = 0.349019607843137;
  double clr_G_diffuse = 0.694117647058824;
  double clr_B_diffuse = 0.905882352941176;

  double temp_clr_R = 0.0;
  double temp_clr_G = 0.0;
  double temp_clr_B = 0.0;

  double camera_point_x = 0.0;
  double camera_point_y = 0.0;
  double camera_point_z = 0.0;

  vec3_t point = vec3_create(NULL);
  vec3_t direction = vec3_create(NULL);
  vec3_t normal_v = vec3_create(NULL);

  appState->camera->get_P(&camera_point_x, &camera_point_y, &camera_point_z);
  appState->camera->cache__get_3d_point__constants(&(appState->wMandel), &(appState->hMandel));

  for (y = 0; y < appState->hMandel; y += 1) {
    // printf("y = %d (of %d)\n", y, appState->hMandel);

    for (x = 0; x < appState->wMandel; x += 1) {
      appState->camera->get_3d_point(
        &x, &y,
        &point[0], &point[1], &point[2]
      );

      direction[0] = point[0] - camera_point_x;
      direction[1] = point[1] - camera_point_y;
      direction[2] = point[2] - camera_point_z;

      normalize(&direction);

      totalDistance = raymarch(&point, &direction);

      if (totalDistance == -1) {
        appState->arrayMandel[i] = 0;
        appState->arrayMandel[i + 1] = 0;
        appState->arrayMandel[i + 2] = 0;
        appState->arrayMandel[i + 3] = 255;
      } else {
        get_normal(&point, &direction, totalDistance, &normal_v);

        temp_coef = max_double(0.0, normal_v[0] * LightSrc_x + normal_v[1] * LightSrc_y + normal_v[2] * LightSrc_z);

        temp_clr_R = clr_R_ambient + clr_R_diffuse * temp_coef;
        temp_clr_G = clr_G_ambient + clr_G_diffuse * temp_coef;
        temp_clr_B = clr_B_ambient + clr_B_diffuse * temp_coef;

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

        appState->arrayMandel[i] = (unsigned int)(255.0 * temp_clr_R);
        appState->arrayMandel[i + 1] = (unsigned int)(255.0 * temp_clr_G);
        appState->arrayMandel[i + 2] = (unsigned int)(255.0 * temp_clr_B);
        appState->arrayMandel[i + 3] = 255;
      }

      i += 4;
    }
  }

  vec3_delete(normal_v);
  vec3_delete(direction);
  vec3_delete(point);
}
