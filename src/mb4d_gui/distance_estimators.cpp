#include <math.h>
#include "utils.hpp"
#include "distance_estimators.hpp"

double simple_mandelbulb_de(AppState* appState, double Px, double Py, double Pz)
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

  for (i = 0; i < appState->simple_mandelbulb_de_params->fractalIters; i += 1) {
    r = sqrt(zX * zX + zY * zY + zZ * zZ);

    if (r > appState->simple_mandelbulb_de_params->bailout) {
      break;
    }

    // Convert to polar coordinates.
    theta = acos(zZ / r);
    phi = atan2(zY, zX);
    dr =  pow(
      r,
      appState->simple_mandelbulb_de_params->Power - 1.0
    ) * appState->simple_mandelbulb_de_params->Power * dr + 1.0;

    // Scale and rotate the point.
    zr = pow(r, appState->simple_mandelbulb_de_params->Power);
    theta = theta * appState->simple_mandelbulb_de_params->Power;
    phi = phi * appState->simple_mandelbulb_de_params->Power;

    // Convert back to Cartesian coordinates.
    zX = zr * sin(theta) * cos(phi) + Px;
    zY = zr * sin(phi) * sin(theta) + Py;
    zZ = zr * cos(theta) + Pz;
  }

  return 0.5 * log(r) * r / dr;
}

double repeated_mandelbulb_de(AppState* appState, double Px, double Py, double Pz)
{
  Px = mod(Px, 6.0) - 3.0;
  Py = mod(Py, 6.0) - 3.0;
  Pz = mod(Pz, 6.0) - 3.0;

  return simple_mandelbulb_de(appState, Px, Py, Pz);
}

const double R = 0.7;

double simple_sphere_de(AppState* appState, double Px, double Py, double Pz)
{
  double length = sqrt(Px * Px + Py * Py + Pz * Pz);

  return max_double(0.0, length - R);
}
