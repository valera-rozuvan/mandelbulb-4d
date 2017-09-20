#include <math.h>
#include "utils.hpp"
#include "color.hpp"

void simpleColorScheme1(
  AppState* appState,
  const double totalDistance,
  const unsigned int fractalIdx,
  const double nX, const double nY, const double nZ
)
{
  double tempCoef;

  double tempClrR;
  double tempClrG;
  double tempClrB;

  if (totalDistance == -1) {
    // Not a fractal point. Set image pixel to background color.
    appState->arrayMandel[fractalIdx] = 0;
    appState->arrayMandel[fractalIdx + 1] = 0;
    appState->arrayMandel[fractalIdx + 2] = 0;
    appState->arrayMandel[fractalIdx + 3] = 255;
  } else {
    // Get a color for the 3D point.
    tempCoef = maxDouble(0.0, nX * appState->lightSrcX + nY * appState->lightSrcY + nZ * appState->lightSrcZ);

    tempClrR = appState->clrAmbientR + appState->clrDiffuseR * tempCoef;
    tempClrG = appState->clrAmbientG + appState->clrDiffuseG * tempCoef;
    tempClrB = appState->clrAmbientB + appState->clrDiffuseB * tempCoef;


    // Make sure that calculated color is within a proper range.
    if (tempClrR < 0) {
      tempClrR = 0;
    } else if (tempClrR > 1.0) {
      tempClrR = 1.0;
    }
    if (tempClrG < 0) {
      tempClrG = 0;
    } else if (tempClrG > 1.0) {
      tempClrG = 1.0;
    }
    if (tempClrB < 0) {
      tempClrB = 0;
    } else if (tempClrB > 1.0) {
      tempClrB = 1.0;
    }


    // Set the image pixel color.
    appState->arrayMandel[fractalIdx] = (unsigned int)(255.0 * tempClrR);
    appState->arrayMandel[fractalIdx + 1] = (unsigned int)(255.0 * tempClrG);
    appState->arrayMandel[fractalIdx + 2] = (unsigned int)(255.0 * tempClrB);
    appState->arrayMandel[fractalIdx + 3] = 255;
  }
}


void simpleColorScheme2(
  AppState* appState,
  const double totalDistance,
  const unsigned int fractalIdx,
  const double nX, const double nY, const double nZ
)
{
  double tempCoef;

  double tempClrR;
  double tempClrG;
  double tempClrB;

  if (totalDistance == -1) {
    // Not a fractal point. Set image pixel to background color.
    appState->arrayMandel[fractalIdx] = 0;
    appState->arrayMandel[fractalIdx + 1] = 0;
    appState->arrayMandel[fractalIdx + 2] = 0;
    appState->arrayMandel[fractalIdx + 3] = 255;
  } else {
    // Get a color for the 3D point.
    tempCoef = maxDouble(0.0, nX * appState->lightSrcX + nY * appState->lightSrcY + nZ * appState->lightSrcZ);

    tempClrR = appState->clrAmbientR + appState->clrDiffuseR * tempCoef;
    tempClrG = appState->clrAmbientG + appState->clrDiffuseG * tempCoef;
    tempClrB = appState->clrAmbientB + appState->clrDiffuseB * tempCoef;

    tempCoef = appState->darkColorCoeff * totalDistance;

    tempClrR = tempClrR / (tempCoef + tempClrR);
    tempClrG = tempClrG / (tempCoef + tempClrG);
    tempClrB = tempClrB / (tempCoef + tempClrB);

    tempCoef = 1.0 / (totalDistance - 1.0);

    tempClrR = tempCoef * (pow(totalDistance, tempClrR) - 1.0);
    tempClrG = tempCoef * (pow(totalDistance, tempClrG) - 1.0);
    tempClrB = tempCoef * (pow(totalDistance, tempClrB) - 1.0);


    // Make sure that calculated color is within a proper range.
    if (tempClrR < 0) {
      tempClrR = 0;
    } else if (tempClrR > 1.0) {
      tempClrR = 1.0;
    }
    if (tempClrG < 0) {
      tempClrG = 0;
    } else if (tempClrG > 1.0) {
      tempClrG = 1.0;
    }
    if (tempClrB < 0) {
      tempClrB = 0;
    } else if (tempClrB > 1.0) {
      tempClrB = 1.0;
    }


    // Set the image pixel color.
    appState->arrayMandel[fractalIdx] = (unsigned int)(255.0 * tempClrR);
    appState->arrayMandel[fractalIdx + 1] = (unsigned int)(255.0 * tempClrG);
    appState->arrayMandel[fractalIdx + 2] = (unsigned int)(255.0 * tempClrB);
    appState->arrayMandel[fractalIdx + 3] = 255;
  }
}
