#include <stdio.h>
#include <stdlib.h>
#include "utils.hpp"
#include "distance_estimators.hpp"
#include "app_state.hpp"

AppState::AppState(const unsigned int wMandel_, const unsigned int hMandel_)
{
  this->is_generating = false;

  // this->DE = simple_sphere_de;
  // this->DE = &repeated_mandelbulb_de;
  this->DE = &simple_mandelbulb_de;

  // this->MaximumRaySteps = 320;
  // this->MinimumDistance = 0.001;
  // this->HalfMinimumDistance = 0.0005;

  this->MaximumRaySteps = 100;
  this->MinimumDistance = 0.01;
  this->HalfMinimumDistance = 0.005;

  this->LightSrc_x = -23.0;
  this->LightSrc_y = 17.0;
  this->LightSrc_z = 12.0;

  normalize(&(this->LightSrc_x), &(this->LightSrc_y), &(this->LightSrc_z));

  // this->dark_color_coeff = 0.02;
  this->dark_color_coeff = 0.3;

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

  // this->camera->set_Px(-2.0);
  // this->camera->set_Pz(5.5);
  // this->camera->set_Py(-0.3);
  // this->camera->set_F(0.8);
  // this->camera->set_beta(35.0);
  // this->camera->recalculate_internals();

  this->camera->set_Px(-3.0);
  this->camera->set_Pz(0.3);
  this->camera->set_Py(-0.3);
  this->camera->set_F(1.0);
  this->camera->set_beta(35.0);
  this->camera->recalculate_internals();

  this->simple_mandelbulb_de_params = new Simple_mandelbulb_de_params();

  // this->simple_mandelbulb_de_params->bailout = 13.7;
  // this->simple_mandelbulb_de_params->Power = 42.08642;
  // this->simple_mandelbulb_de_params->fractalIters = 1000;

  this->simple_mandelbulb_de_params->bailout = 2;
  this->simple_mandelbulb_de_params->Power = 8.0;
  this->simple_mandelbulb_de_params->fractalIters = 100;
}

AppState::~AppState(void)
{
  fprintf(stdout, "AppState::~AppState destructor.\n");

  delete this->parallel;
  delete this->arrayMandel;
  delete this->simple_mandelbulb_de_params;
  delete this->camera;
}
