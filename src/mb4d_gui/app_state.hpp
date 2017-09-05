#ifndef APP_STATE_H
#define APP_STATE_H

#include <stdbool.h>
#include "parallel.hpp"
#include "mcamera.hpp"

class AppState {
  public:
    bool is_generating;

    unsigned char* arrayMandel;

    unsigned int wMandel;
    unsigned int hMandel;

    Parallel* parallel;
    MCamera* camera;

    // fractal_params ??

    AppState(const unsigned int, const unsigned int);
    ~AppState(void);
};

#endif
