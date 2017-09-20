#include <stdio.h>
#include <stdlib.h>
#include "utils.hpp"
#include "distance_estimators.hpp"
#include "app_state.hpp"

AppState::AppState(const unsigned int wMandel_, const unsigned int hMandel_)
{
  // this->DE = &simpleSphereDe;
  this->DE = &repeatedMandelbulbDe;
  // this->DE = &simpleMandelbulbDe;

  // this->maximumRaySteps = 320;
  // this->minimumDistance = 0.001;
  // this->halfMinimumDistance = 0.0005;

  this->maximumRaySteps = 500;
  this->minimumDistance = 0.001;
  this->halfMinimumDistance = 0.0005;

  this->lightSrcX = -23.0;
  this->lightSrcY = 17.0;
  this->lightSrcZ = 12.0;

  normalize(&(this->lightSrcX), &(this->lightSrcY), &(this->lightSrcZ));

  // this->darkColorCoeff = 0.02;
  this->darkColorCoeff = 0.03;

  this->clrAmbientR = 0.019607843137255;
  this->clrAmbientG = 0.145098039215686;
  this->clrAmbientB = 0.376470588235294;

  this->clrDiffuseR = 0.349019607843137;
  this->clrDiffuseG = 0.694117647058824;
  this->clrDiffuseB = 0.905882352941176;

  this->wMandel = wMandel_;
  this->hMandel = hMandel_;

  this->aspectRatioMandel = ((double)this->wMandel) / ((double)this->hMandel);

  this->arrayMandel = (unsigned char *)calloc(sizeof(unsigned char) * this->wMandel * this->hMandel * 4, sizeof(unsigned char));

  this->camera = new MCamera();
  this->parallel = new Parallel(this, 4, 85);

  // this->camera->set_Px(-2.0);
  // this->camera->set_Pz(5.5);
  // this->camera->set_Py(-0.3);
  // this->camera->set_F(0.8);
  // this->camera->set_beta(35.0);
  // this->camera->recalculate_internals();

  this->camera->set_Px(-3.0);
  this->camera->set_Pz(1.3);
  this->camera->set_Py(-1.5);
  this->camera->set_F(1.0);
  this->camera->set_beta(35.0);
  this->camera->recalculate_internals();

  this->camera->cache__get_3d_point__constants(this->wMandel, this->hMandel);

  this->simpleMandelbulbDeParams = new SimpleMandelbulbDeParams();

  // this->simpleMandelbulbDeParams->bailout = 13.7;
  // this->simpleMandelbulbDeParams->power = 42.08642;
  // this->simpleMandelbulbDeParams->fractalIters = 1000;

  this->simpleMandelbulbDeParams->bailout = 2;
  this->simpleMandelbulbDeParams->power = 8.0;
  this->simpleMandelbulbDeParams->fractalIters = 100;
}

AppState::~AppState(void)
{
  fprintf(stdout, "AppState::~AppState destructor.\n");

  fprintf(stdout, "--> calling Parallel destructor...\n");
  delete this->parallel;

  fprintf(stdout, "--> deleting arrayMandel array...\n");
  free(this->arrayMandel);

  fprintf(stdout, "--> calling SimpleMandelbulbDeParams destructor...\n");
  delete this->simpleMandelbulbDeParams;

  fprintf(stdout, "--> calling Camera destructor...\n");
  delete this->camera;
}
