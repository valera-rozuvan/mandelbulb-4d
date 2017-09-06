#ifndef MCAMERA_HPP
#define MCAMERA_HPP

class MCamera {
  private:
    // Camera position.
    double Px;
    double Py;
    double Pz;

    // Camera up vector.
    double UPx;
    double UPy;
    double UPz;

    // Camera right vector.
    double RIGHTx;
    double RIGHTy;
    double RIGHTz;

    // Camera look at vector.
    double Lx;
    double Ly;
    double Lz;

    // 3D point of image plane's center.
    double CIMGPx;
    double CIMGPy;
    double CIMGPz;

    // 3D point of top-left corner of image plane.
    double TLx;
    double TLy;
    double TLz;

    // Image plane dimensions.
    double IMGP_width;
    double IMGP_height;

    // Image plane aspect ratio.
    double IMGP_AR;

    // "Focal" distance (distance from camera position to image plane).
    double F;

    // Camera view angles (alfa - x axis image plane, beta - y axis image plane).
    double alfa;
    double beta;

    // Some constants are cached.
    double get_3d_point__constant__x_offset;
    double get_3d_point__constant__y_offset;

    double get_3d_point__constant__c1;
    double get_3d_point__constant__c2;

    void calculate_RIGHT(void);
    void calculate_CIMGP(void);
    void calculate_IMGP_dim(void);
    void caluclate_TL(void);
    void caluclate_AR(void);

  public:
    void set_P(double, double, double);
    void set_Px(double);
    void set_Py(double);
    void set_Pz(double);

    void set_UP(double, double, double);
    void set_UPx(double);
    void set_UPy(double);
    void set_UPz(double);

    void set_L(double, double, double);
    void set_Lx(double);
    void set_Ly(double);
    void set_Lz(double);

    void set_F(double);

    void set_alfa(double);
    void set_beta(double);

    void recalculate_internals(void);

    void cache__get_3d_point__constants(const unsigned int*, const unsigned int*);

    void get_3d_point(unsigned int*, unsigned int*, double*, double*, double*);
    void get_P(double*, double*, double*);

    // Constructor. Initialize all private members.
    MCamera(void);

    ~MCamera(void);

};

#endif
