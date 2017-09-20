#ifndef DISTANCE_ESTIMATOR_HPP
#define DISTANCE_ESTIMATOR_HPP

#include "app_state.hpp"

double simpleMandelbulbDe(AppState*, double, double, double);
double repeatedMandelbulbDe(AppState*, double, double, double);
double simpleSphereDe(AppState*, double, double, double);

#endif // DISTANCE_ESTIMATOR_HPP
