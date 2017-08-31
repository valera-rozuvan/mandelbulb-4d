#include <cstddef>
#include <math.h>
#include "mcamera.hpp"
#include "mandelbulb.hpp"
#include "vec3_math.hpp"

const int MaximumRaySteps = 200;
const double MinimumDistance = 0.001;
const double R = 5.0;

double max_double(double a, double b)
{
  if (a > b) {
    return a;
  }

  return b;
}

void normalize(double* x, double* y, double* z) {
  double length = sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));

  *x = (*x) / length;
  *y = (*y) / length;
  *z = (*z) / length;
}

// double DistanceEstimator(double x, double y, double z)
// {
//   double length = sqrt(x * x + y * y + z * z);

//   return max_double(0.0, length - R);
// }

double DistanceEstimator(double xx, double yy, double zz) {
  const double bailout = 10.0;
  const double Power = 8.0;
  const double fractalIters = 1000;

  double zX = xx;
  double zY = yy;
  double zZ = zz;

  double dr = 1.0;
  double r = 0.0;

  for (int i = 0; i < fractalIters; i += 1) {
    r = sqrt(zX * zX + zY * zY + zZ * zZ);
    if (r > bailout) {
      break;
    }

    // Convert to polar coordinates.
    double theta = acos(zZ / r);
    double phi = atan2(zY, zX);
    dr =  pow(r, Power - 1.0) * Power * dr + 1.0;

    // Scale and rotate the point.
    double zr = pow(r, Power);
    theta = theta * Power;
    phi = phi * Power;

    // Convert back to Cartesian coordinates.
    zX = zr * sin(theta) * cos(phi) + xx;
    zY = zr * sin(phi) * sin(theta) + yy;
    zZ = zr * cos(theta) + zz;
  }

  return 0.5 * log(r) * r / dr;
}

void get_normal(vec3_t from, vec3_t direction, double totalDistance, vec3_t* normal_v) {
  double Px = from[0] + totalDistance * direction[0];
  double Py = from[1] + totalDistance * direction[1];
  double Pz = from[2] + totalDistance * direction[2];

  double Nx = DistanceEstimator(Px + 0.5 * MinimumDistance, Py, Pz) - DistanceEstimator(Px - 0.5 * MinimumDistance, Py, Pz);
  double Ny = DistanceEstimator(Px, Py + 0.5 * MinimumDistance, Pz) - DistanceEstimator(Px, Py - 0.5 * MinimumDistance, Pz);
  double Nz = DistanceEstimator(Px, Py, Pz + 0.5 * MinimumDistance) - DistanceEstimator(Px, Py, Pz - 0.5 * MinimumDistance);

  normalize(&Nx, &Ny, &Nz);

  (*normal_v)[0] = Nx;
  (*normal_v)[1] = Ny;
  (*normal_v)[2] = Nz;
}

double trace(vec3_t from, vec3_t direction)
{
  double totalDistance = 0.0;
  double distance = 0.0;
  int steps = 0;
  double ret_value = -1.0;

  vec3_t point = vec3_create(NULL);
  vec3_t scaledDir = vec3_create(NULL);

  for (steps = 0; steps < MaximumRaySteps; steps += 1) {
    vec3_set(from, point);
    vec3_scale(direction, totalDistance, scaledDir);
    vec3_add(point, scaledDir, NULL);

    distance = DistanceEstimator(point[0], point[1], point[2]);
    totalDistance += distance;

    if (distance < MinimumDistance) {
      ret_value = totalDistance;

      break;
    }
  }

  vec3_delete(point);
  vec3_delete(scaledDir);

  return ret_value;
}

void generateFractal(
  unsigned char *arrayMandel,
  const unsigned int wMandel, const unsigned int hMandel,
  MCamera* camera
) {
  double totalDistance = 0;
  unsigned int i = 0;
  unsigned int x = 0;
  unsigned int y = 0;

  double LSx = -23.0;
  double LSy = 17.0;
  double LSz = 12.0;

  normalize(&LSx, &LSy, &LSz);

  double color_Ra = 0.1;
  double color_Ga = 0.3;
  double color_Ba = 0.1;

  double color_Rd = 0.7;
  double color_Gd = 0.3;
  double color_Bd = 0.9;

  double temp_color_R = 0.0;
  double temp_color_G = 0.0;
  double temp_color_B = 0.0;

  vec3_t camera_point = vec3_create(NULL);
  vec3_t point = vec3_create(NULL);
  vec3_t direction = vec3_create(NULL);
  vec3_t normal_v = vec3_create(NULL);

  camera->get_P(&camera_point[0], &camera_point[1], &camera_point[2]);
  camera->cache__get_3d_point__constants(&wMandel, &hMandel);

  for (y = 0; y < hMandel; y += 1) {
    for (x = 0; x < wMandel; x += 1) {
      camera->get_3d_point(
        &x, &y,
        &point[0], &point[1], &point[2]
      );

      direction[0] = point[0] - camera_point[0];
      direction[1] = point[1] - camera_point[1];
      direction[2] = point[2] - camera_point[2];

      normalize(&direction[0], &direction[1], &direction[2]);

      totalDistance = trace(point, direction);

      if (totalDistance == -1) {
        arrayMandel[i] = 0;
        arrayMandel[i + 1] = 0;
        arrayMandel[i + 2] = 0;
        arrayMandel[i + 3] = 255;
      } else {
        get_normal(point, direction, totalDistance, &normal_v);

        temp_color_R = color_Ra + color_Rd * max_double(0.0, normal_v[0] * LSx + normal_v[1] * LSy +normal_v[2] * LSz);
        temp_color_G = color_Ga + color_Gd * max_double(0.0, normal_v[0] * LSx + normal_v[1] * LSy +normal_v[2] * LSz);
        temp_color_B = color_Ba + color_Bd * max_double(0.0, normal_v[0] * LSx + normal_v[1] * LSy +normal_v[2] * LSz);

        if (temp_color_R < 0) {
          temp_color_R = 0;
        } else if (temp_color_R > 1.0) {
          temp_color_R = 1.0;
        }
        if (temp_color_G < 0) {
          temp_color_G = 0;
        } else if (temp_color_G > 1.0) {
          temp_color_G = 1.0;
        }
        if (temp_color_B < 0) {
          temp_color_B = 0;
        } else if (temp_color_B > 1.0) {
          temp_color_B = 1.0;
        }

        arrayMandel[i] = (unsigned int)(255.0 * temp_color_R);
        arrayMandel[i + 1] = (unsigned int)(255.0 * temp_color_G);
        arrayMandel[i + 2] = (unsigned int)(255.0 * temp_color_B);
        arrayMandel[i + 3] = 255;
      }

      i += 4;
    }
  }

  vec3_delete(normal_v);
  vec3_delete(direction);
  vec3_delete(point);
  vec3_delete(camera_point);
}
