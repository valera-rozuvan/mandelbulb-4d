#include <stdio.h>
#include <stdlib.h>
#include "utils.hpp"
#include "app_state.hpp"

AppState::AppState(const unsigned int wMandel_, const unsigned int hMandel_)
{
  this->is_generating = false;

  this->LightSrc_x = -23.0;
  this->LightSrc_y = 17.0;
  this->LightSrc_z = 12.0;

  normalize(&(this->LightSrc_x), &(this->LightSrc_y), &(this->LightSrc_z));

  this->clr_R_ambient = 0.019607843137255;
  this->clr_G_ambient = 0.145098039215686;
  this->clr_B_ambient = 0.376470588235294;

  this->clr_R_diffuse = 0.349019607843137;
  this->clr_G_diffuse = 0.694117647058824;
  this->clr_B_diffuse = 0.905882352941176;

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
