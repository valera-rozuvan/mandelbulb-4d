unsigned int mandel(double x0, double y0, double z0)
{
  const unsigned int n = 8;

  double nD = 0.0;

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  double r = 0.0;
  double theta = 0.0;
  double phi = 0.0;

  unsigned int i = 0;

  for (i = 0; i <= 32; i += 1) {
    nD = (double)n;

    r = sqrt(x * x + y * y + z * z);
    theta = atan2(sqrt(x * x + y * y), z);
    phi = atan2(y, x);

    x = pow(r, nD) * sin(theta * nD) * cos(phi * nD) + x0;
    y = pow(r, nD) * sin(theta * nD) * sin(phi * nD) + y0;
    z = pow(r, nD) * cos(theta * nD)                 + z0;

    if (pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0) > 2.0) {
      return 256 - (i * 4);
    }
  }

  return 0;
}

void generateMandel(int layer, unsigned char *arrayMandel, const unsigned int wMandel, const unsigned int hMandel) {
  unsigned int m = 0;
  unsigned int i = 0;

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  for (unsigned int yy = 0; yy < hMandel; ++yy) {
    for (unsigned int xx = 0; xx < wMandel; ++xx) {
      x = 4.0 * ((double)xx - ((double)wMandel / 2.0)) / (double)wMandel;
      y = 4.0 * ((double)yy - ((double)hMandel / 2.0)) / (double)hMandel;
      z = (float)layer / 50.0;

      m = mandel(x, y, z);

      arrayMandel[i] = m;
      arrayMandel[i + 1] = m;
      arrayMandel[i + 2] = m;
      arrayMandel[i + 3] = 255;

      i += 4;
    }
  }
}
