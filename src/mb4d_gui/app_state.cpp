#include <stdio.h>
#include <stdlib.h>
#include "app_state.hpp"

AppState::AppState(const unsigned int wMandel_, const unsigned int hMandel_)
{
  this->is_generating = false;

  this->wMandel = wMandel_;
  this->hMandel = hMandel_;

  this->arrayMandel = (unsigned char *)calloc(sizeof(unsigned char) * this->wMandel * this->hMandel * 4, sizeof(unsigned char));

  this->camera = new MCamera();
  this->parallel = new Parallel();
}

AppState::~AppState(void)
{
  fprintf(stdout, "AppState::~AppState destructor.\n");

  delete this->parallel;
  delete this->arrayMandel;
  delete this->camera;
}
