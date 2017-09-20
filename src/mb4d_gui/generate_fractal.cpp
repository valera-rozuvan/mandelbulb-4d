#include "mcamera.hpp"
#include "utils.hpp"
#include "color.hpp"
#include "distance_estimators.hpp"
#include "generate_fractal.hpp"

/*
 *
 * getNormal(AppState, P, N) - calculates normal vector N at point P
 *
 * AppState - application state configuration
 * pX, pY, pZ - coordinates of point P
 * nX, nY, nZ - components of vector N
 *
 */
void getNormal(
  AppState* const appState,
  const double pX, const double pY, const double pZ,
  double* const nX, double* const nY, double* const nZ
)
{
  *nX = (*(appState->DE))(appState, pX + appState->halfMinimumDistance, pY, pZ) -
        (*(appState->DE))(appState, pX - appState->halfMinimumDistance, pY, pZ);
  *nY = (*(appState->DE))(appState, pX, pY + appState->halfMinimumDistance, pZ) -
        (*(appState->DE))(appState, pX, pY - appState->halfMinimumDistance, pZ);
  *nZ = (*(appState->DE))(appState, pX, pY, pZ + appState->halfMinimumDistance) -
        (*(appState->DE))(appState, pX, pY, pZ - appState->halfMinimumDistance);

  normalize(nX, nY, nZ);
}

/*
 *
 * raymarch(AppState, P, D) - ray marching from point P in direction D
 *
 * AppState - application state configuration
 * pX, pY, pZ - coordinates of point P
 * dX, dY, dZ - components of vector D
 *
 * Returns distance to found point; if nothing is found, returns -1.
 *
 */
double raymarch(
  AppState* const appState,
  const double pX, const double pY, const double pZ,
  const double dX, const double dY, const double dZ
)
{
  double totalDistance;
  double distance;

  unsigned int steps;

  double pX_;
  double pY_;
  double pZ_;

  totalDistance = 0.0;

  for (steps = 0; steps < appState->maximumRaySteps; steps += 1) {
    pX_ = pX + totalDistance * dX;
    pY_ = pY + totalDistance * dY;
    pZ_ = pZ + totalDistance * dZ;

    distance = (*(appState->DE))(appState, pX_, pY_, pZ_);
    totalDistance += distance;

    if (distance < appState->minimumDistance) {
      return totalDistance;
    }
  }

  return -1.0;
}

unsigned int getFractalIdx(AppState const * const appState, const unsigned int x, const unsigned int y)
{
  return 4 * (appState->wMandel * y + x);
}

void generateFractal(AppState* const appState, WorkQueueItem const * const workItem)
{
  unsigned int fractalIdx;

  double totalDistance;

  unsigned int x;
  unsigned int y;

  double camPx, camPy, camPz;

  double pX, pY, pZ;
  double dX, dY, dZ;
  double nX, nY, nZ;

  unsigned int startX = workItem->startX;
  unsigned int endX = workItem->endX;

  unsigned int startY = workItem->startY;
  unsigned int endY = workItem->endY;

  appState->camera->get_P(&camPx, &camPy, &camPz);

  for (y = startY; y <= endY; y += 1) {

    for (x = startX; x <= endX; x += 1) {
      // If parallel instance is terminating, return back to the thread
      // worker function.
      if (appState->parallel->selfDestructing == true) {
        return;
      }


      // Calculate fractal array 1D offset.
      fractalIdx = getFractalIdx(appState, x, y);


      // Calculate 3D point in image plane.
      appState->camera->get_3d_point(x, y, &pX, &pY, &pZ);


      // Calculate direction for ray marching.
      dX = pX - camPx;
      dY = pY - camPy;
      dZ = pZ - camPz;

      normalize(&dX, &dY, &dZ);


      // Ray marching.
      totalDistance = raymarch(appState, pX, pY, pZ, dX, dY, dZ);


      // Update normal vector for current point.
      if (totalDistance == -1) {
        nX = 0.0;
        nY = 0.0;
        nZ = 0.0;
      } else {
        // Calculate 3D point of fractal.
        pX += totalDistance * dX;
        pY += totalDistance * dY;
        pZ += totalDistance * dZ;


        // Get normal vector for 3D point of fractal.
        getNormal(appState, pX, pY, pZ, &nX, &nY, &nZ);
      }


      // Color our 2D pixel.
      simpleColorScheme2(appState, totalDistance, fractalIdx, nX, nY, nZ);
    }
  }
}
