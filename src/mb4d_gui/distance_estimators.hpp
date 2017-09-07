#ifndef DISTANCE_ESTIMATOR_HPP
#define DISTANCE_ESTIMATOR_HPP

#include "app_state.hpp"

double simple_mandelbulb_de(AppState*, double, double, double);
double repeated_mandelbulb_de(AppState*, double, double, double);
double simple_sphere_de(AppState*, double, double, double);

#endif // DISTANCE_ESTIMATOR_HPP
