#include <math.h>
#include "utils.hpp"
#include "color.hpp"

void simpleColorScheme1(
  AppState* appState,
  const double totalDistance,
  const unsigned int fractalIdx,
  const double Nx, const double Ny, const double Nz
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
    tempCoef = maxDouble(0.0, Nx * appState->LightSrc_x + Ny * appState->LightSrc_y + Nz * appState->LightSrc_z);

    tempClrR = appState->clr_R_ambient + appState->clr_R_diffuse * tempCoef;
    tempClrG = appState->clr_G_ambient + appState->clr_G_diffuse * tempCoef;
    tempClrB = appState->clr_B_ambient + appState->clr_B_diffuse * tempCoef;


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
  const double Nx, const double Ny, const double Nz
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
    tempCoef = maxDouble(0.0, Nx * appState->LightSrc_x + Ny * appState->LightSrc_y + Nz * appState->LightSrc_z);

    tempClrR = appState->clr_R_ambient + appState->clr_R_diffuse * tempCoef;
    tempClrG = appState->clr_G_ambient + appState->clr_G_diffuse * tempCoef;
    tempClrB = appState->clr_B_ambient + appState->clr_B_diffuse * tempCoef;

    tempCoef = appState->dark_color_coeff * totalDistance;

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
