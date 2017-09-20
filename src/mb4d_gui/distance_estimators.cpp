#include <math.h>
#include "utils.hpp"
#include "distance_estimators.hpp"

double simpleMandelbulbDe(AppState* appState, double pX, double pY, double pZ)
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

  zX = pX;
  zY = pY;
  zZ = pZ;

  for (i = 0; i < appState->simpleMandelbulbDeParams->fractalIters; i += 1) {
    r = sqrt(zX * zX + zY * zY + zZ * zZ);

    if (r > appState->simpleMandelbulbDeParams->bailout) {
      break;
    }

    // Convert to polar coordinates.
    theta = acos(zZ / r);
    phi = atan2(zY, zX);
    dr = pow(
      r,
      appState->simpleMandelbulbDeParams->power - 1.0
    ) * appState->simpleMandelbulbDeParams->power * dr + 1.0;

    // Scale and rotate the point.
    zr = pow(r, appState->simpleMandelbulbDeParams->power);
    theta = theta * appState->simpleMandelbulbDeParams->power;
    phi = phi * appState->simpleMandelbulbDeParams->power;

    // Convert back to Cartesian coordinates.
    zX = zr * sin(theta) * cos(phi) + pX;
    zY = zr * sin(phi) * sin(theta) + pY;
    zZ = zr * cos(theta) + pZ;
  }

  return 0.5 * log(r) * r / dr;
}

double repeatedMandelbulbDe(AppState* appState, double pX, double pY, double pZ)
{
  pX = mod(pX, 6.0) - 3.0;
  pY = mod(pY, 6.0) - 3.0;
  pZ = mod(pZ, 6.0) - 3.0;

  return simpleMandelbulbDe(appState, pX, pY, pZ);
}

const double R = 0.7;

double simpleSphereDe(AppState* appState, double pX, double pY, double pZ)
{
  double length = sqrt(pX * pX + pY * pY + pZ * pZ);

  return maxDouble(0.0, length - R);
}
