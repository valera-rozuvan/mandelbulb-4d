#ifndef APP_STATE_H
#define APP_STATE_H

#include <stdbool.h>
#include "parallel.hpp"
#include "mcamera.hpp"
#include "de_params/simple_mandelbulb_de_params.hpp"

class AppState {
  public:
    double darkColorCoeff;

    double (*DE)(AppState*, double, double, double);

    SimpleMandelbulbDeParams* simpleMandelbulbDeParams;

    unsigned int maximumRaySteps;
    double minimumDistance;
    double halfMinimumDistance;

    double lightSrcX;
    double lightSrcY;
    double lightSrcZ;

    double clrAmbientR;
    double clrAmbientG;
    double clrAmbientB;

    double clrDiffuseR;
    double clrDiffuseG;
    double clrDiffuseB;

    unsigned char* arrayMandel;

    unsigned int wMandel;
    unsigned int hMandel;

    double aspectRatioMandel;

    Parallel* parallel;
    MCamera* camera;

    AppState(const unsigned int, const unsigned int);
    ~AppState(void);
};

#endif // APP_STATE_H
