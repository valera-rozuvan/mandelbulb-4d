#include <math.h>
#include "utils.hpp"
#include "color.hpp"

void simple_color_scheme1(
  AppState* appState,
  double totalDistance,
  unsigned int fractalIdx,
  double Nx, double Ny, double Nz
)
{
  double temp_coef;
  double temp_clr_R, temp_clr_G, temp_clr_B;

  if (totalDistance == -1) {
    // Not a fractal point. Set image pixel to background color.
    appState->arrayMandel[fractalIdx] = 0;
    appState->arrayMandel[fractalIdx + 1] = 0;
    appState->arrayMandel[fractalIdx + 2] = 0;
    appState->arrayMandel[fractalIdx + 3] = 255;
  } else {
    // Get a color for the 3D point.
    temp_coef = max_double(0.0, Nx * appState->LightSrc_x + Ny * appState->LightSrc_y + Nz * appState->LightSrc_z);

    temp_clr_R = appState->clr_R_ambient + appState->clr_R_diffuse * temp_coef;
    temp_clr_G = appState->clr_G_ambient + appState->clr_G_diffuse * temp_coef;
    temp_clr_B = appState->clr_B_ambient + appState->clr_B_diffuse * temp_coef;


    // Make sure that calculated color is within a proper range.
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
    appState->arrayMandel[fractalIdx] = (unsigned int)(255.0 * temp_clr_R);
    appState->arrayMandel[fractalIdx + 1] = (unsigned int)(255.0 * temp_clr_G);
    appState->arrayMandel[fractalIdx + 2] = (unsigned int)(255.0 * temp_clr_B);
    appState->arrayMandel[fractalIdx + 3] = 255;
  }
}


void simple_color_scheme2(
  AppState* appState,
  double totalDistance,
  unsigned int fractalIdx,
  double Nx, double Ny, double Nz
)
{
  double temp_coef;
  double temp_clr_R, temp_clr_G, temp_clr_B;

  if (totalDistance == -1) {
    // Not a fractal point. Set image pixel to background color.
    appState->arrayMandel[fractalIdx] = 0;
    appState->arrayMandel[fractalIdx + 1] = 0;
    appState->arrayMandel[fractalIdx + 2] = 0;
    appState->arrayMandel[fractalIdx + 3] = 255;
  } else {
    // Get a color for the 3D point.
    temp_coef = max_double(0.0, Nx * appState->LightSrc_x + Ny * appState->LightSrc_y + Nz * appState->LightSrc_z);

    temp_clr_R = appState->clr_R_ambient + appState->clr_R_diffuse * temp_coef;
    temp_clr_G = appState->clr_G_ambient + appState->clr_G_diffuse * temp_coef;
    temp_clr_B = appState->clr_B_ambient + appState->clr_B_diffuse * temp_coef;

    temp_coef = 0.02 * totalDistance;

    temp_clr_R = temp_clr_R / (temp_coef + temp_clr_R);
    temp_clr_G = temp_clr_G / (temp_coef + temp_clr_G);
    temp_clr_B = temp_clr_B / (temp_coef + temp_clr_B);

    temp_coef = 1.0 / (totalDistance - 1.0);

    temp_clr_R = temp_coef * (pow(totalDistance, temp_clr_R) - 1.0);
    temp_clr_G = temp_coef * (pow(totalDistance, temp_clr_G) - 1.0);
    temp_clr_B = temp_coef * (pow(totalDistance, temp_clr_B) - 1.0);


    // Make sure that calculated color is within a proper range.
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
    appState->arrayMandel[fractalIdx] = (unsigned int)(255.0 * temp_clr_R);
    appState->arrayMandel[fractalIdx + 1] = (unsigned int)(255.0 * temp_clr_G);
    appState->arrayMandel[fractalIdx + 2] = (unsigned int)(255.0 * temp_clr_B);
    appState->arrayMandel[fractalIdx + 3] = 255;
  }
}
