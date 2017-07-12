#ifndef DRAWING2_HPP
#define DRAWING2_HPP

void draw_mandelbulb(
  struct nk_context* ctx,
  unsigned int texture,
  unsigned char arrayMandel[],
  unsigned int WIDTH_IMG, unsigned int HEIGHT_IMG
);

#endif // DRAWING2_HPP
