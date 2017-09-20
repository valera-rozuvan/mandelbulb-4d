#include <stdio.h>
#include <cmath>
#include "mcamera.hpp"
#include "utils.hpp"

const double PI_ = 3.14159265358979323846;

void MCamera::calculate_RIGHT(void) {
  // Vector cross product.
  this->RIGHTx = -1.0 * this->UPy * this->Lz + this->UPz * this->Ly;
  this->RIGHTy = -1.0 * this->UPz * this->Lx + this->UPx * this->Lz;
  this->RIGHTz = -1.0 * this->UPx * this->Ly + this->UPy * this->Lx;

  normalize(&this->RIGHTx, &this->RIGHTy, &this->RIGHTz);
}

void MCamera::calculate_CIMGP(void) {
  this->CIMGPx = this->Px + this->F * this->Lx;
  this->CIMGPy = this->Py + this->F * this->Ly;
  this->CIMGPz = this->Pz + this->F * this->Lz;
}

void MCamera::calculate_IMGP_dim(void) {
  this->IMGP_width = 2.0 * this->F * tan(this->beta);
  this->IMGP_height = 2.0 * this->F * tan(this->alfa);
}

void MCamera::caluclate_TL(void) {
  this->TLx = this->CIMGPx + 0.5 * this->IMGP_height * this->UPx - 0.5 * this->IMGP_width * this->RIGHTx;
  this->TLy = this->CIMGPy + 0.5 * this->IMGP_height * this->UPy - 0.5 * this->IMGP_width * this->RIGHTy;
  this->TLz = this->CIMGPz + 0.5 * this->IMGP_height * this->UPz - 0.5 * this->IMGP_width * this->RIGHTz;
}

void MCamera::caluclate_AR(void) {
  this->IMGP_AR = this->IMGP_width / this->IMGP_height;
}

void MCamera::set_P(double Px_, double Py_, double Pz_) {
  this->Px = Px_;
  this->Py = Py_;
  this->Pz = Pz_;
}

void MCamera::set_Px(double Px_) {
  this->Px = Px_;
}

void MCamera::set_Py(double Py_) {
  this->Py = Py_;
}

void MCamera::set_Pz(double Pz_) {
  this->Pz = Pz_;
}

void MCamera::set_UP(double UPx_, double UPy_, double UPz_) {
  this->UPx = UPx_;
  this->UPy = UPy_;
  this->UPz = UPz_;
}

void MCamera::set_UPx(double UPx_) {
  this->UPx = UPx_;
}

void MCamera::set_UPy(double UPy_) {
  this->UPy = UPy_;
}

void MCamera::set_UPz(double UPz_) {
  this->UPz = UPz_;
}

void MCamera::set_L(double Lx_, double Ly_, double Lz_) {
  this->Lx = Lx_;
  this->Ly = Ly_;
  this->Lz = Lz_;
}

void MCamera::set_Lx(double Lx_) {
  this->Lx = Lx_;
}

void MCamera::set_Ly(double Ly_) {
  this->Ly = Ly_;
}

void MCamera::set_Lz(double Lz_) {
  this->Lz = Lz_;
}

void MCamera::set_F(double F_) {
  this->F = F_;
}

void MCamera::set_alfa(double alfa_) {
  this->alfa = (alfa_ * PI_) / 180.0;
}

void MCamera::set_beta(double beta_) {
  this->beta = (beta_ * PI_) / 180.0;
}

void MCamera::recalculate_internals(void) {
  this->calculate_RIGHT();
  this->calculate_CIMGP();
  this->calculate_IMGP_dim();
  this->caluclate_TL();
  this->caluclate_AR();
}

void MCamera::cache__get_3d_point__constants(unsigned int width, unsigned int height) {
  double AR_2d_img = ((double)width) / ((double)height);
  double x_offset = 0.0;
  double y_offset = 0.0;
  double c1 = 0.0;
  double c2 = 0.0;

  // We assume that two Aspect Ratios are the same, if their difference is less
  // than some very small EPS number.
  if (std::abs(this->IMGP_AR - AR_2d_img) <= 0.0000000001) {
    // Do nothing for now.
  } else if (this->IMGP_AR > AR_2d_img) {
    x_offset = 0.5 * this->IMGP_width - 0.5 * ((((double)width) * this->IMGP_height) / ((double)height));
  } else if (this->IMGP_AR < AR_2d_img) {
    y_offset = 0.5 * this->IMGP_height - 0.5 * ((((double)height) * this->IMGP_width) / ((double)width));
  }

  c1 = ((this->IMGP_width - 2.0 * x_offset) / ((double)width));
  c2 = ((this->IMGP_height - 2.0 * y_offset) / ((double)height));

  this->get_3d_point__constant__x_offset = x_offset;
  this->get_3d_point__constant__y_offset = y_offset;

  this->get_3d_point__constant__c1 = c1;
  this->get_3d_point__constant__c2 = c2;
}

void MCamera::get_3d_point(
  unsigned int x_2d, unsigned int y_2d,
  double* x_3d, double* y_3d, double* z_3d
) {
  double mult1 = this->get_3d_point__constant__x_offset + ((double)x_2d) * this->get_3d_point__constant__c1;
  double mult2 = this->get_3d_point__constant__y_offset + ((double)y_2d) * this->get_3d_point__constant__c2;

  *x_3d = this->TLx + this->RIGHTx * mult1 - this->UPx * mult2;
  *y_3d = this->TLy + this->RIGHTy * mult1 - this->UPy * mult2;
  *z_3d = this->TLz + this->RIGHTz * mult1 - this->UPz * mult2;
}

void MCamera::get_P(double* Px_, double* Py_, double* Pz_) {
  *Px_ = this->Px;
  *Py_ = this->Py;
  *Pz_ = this->Pz;
}

// Setting camera defaults on object instantiation.
MCamera::MCamera(void) {
  // Camera positioned at (0, 0, 0).
  this->Px = 0.0;
  this->Py = 0.0;
  this->Pz = 0.0;

  // Camera UP direction is along the z-axis.
  this->UPx = 0.0;
  this->UPy = 0.0;
  this->UPz = 1.0;

  // Camera is looking along the x-axis.
  this->Lx = 1.0;
  this->Ly = 0.0;
  this->Lz = 0.0;

  // Set distance from camera to image plane.
  this->F = 5.0;

  // Set both camera viewing angles.
  this->alfa = (45.0 * PI_) / 180.0;
  this->beta = (45.0 * PI_) / 180.0;

  this->recalculate_internals();
}

MCamera::~MCamera()
{
  fprintf(stdout, "MCamera::~MCamera destructor.\n");
}
