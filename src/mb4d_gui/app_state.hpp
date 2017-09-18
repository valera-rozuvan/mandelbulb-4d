#ifndef APP_STATE_H
#define APP_STATE_H

#include <stdbool.h>
#include "parallel.hpp"
#include "mcamera.hpp"
#include "de_params/simple_mandelbulb_de_params.hpp"

class AppState {
  public:
    bool is_generating;

    double dark_color_coeff;

    double (*DE)(AppState*, double, double, double);

    Simple_mandelbulb_de_params* simple_mandelbulb_de_params;

    unsigned int MaximumRaySteps;
    double MinimumDistance;
    double HalfMinimumDistance;

    double LightSrc_x;
    double LightSrc_y;
    double LightSrc_z;

    double clr_R_ambient;
    double clr_G_ambient;
    double clr_B_ambient;

    double clr_R_diffuse;
    double clr_G_diffuse;
    double clr_B_diffuse;

    unsigned char* arrayMandel;

    unsigned int wMandel;
    unsigned int hMandel;

    double aspect_ratio_mandel;

    Parallel* parallel;
    MCamera* camera;

    // fractal_params ??

    AppState(const unsigned int, const unsigned int);
    ~AppState(void);
};

#endif
