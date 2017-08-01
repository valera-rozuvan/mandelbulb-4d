#ifndef MANDELBULB_HPP
#define MANDELBULB_HPP

void generateFractal(
  int layer,
  unsigned char *arrayMandel,
  const unsigned int wMandel, const unsigned int hMandel
);

void generateMandel(
  int layer,
  unsigned char *arrayMandel,
  const unsigned int wMandel, const unsigned int hMandel
);

#endif // MANDELBULB_HPP
