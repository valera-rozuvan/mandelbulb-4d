#ifndef DRAW_MANDEL_WND_HPP
#define DRAW_MANDEL_WND_HPP

void drawMandelWnd(
  struct nk_context *ctx,
  unsigned int *texture,
  const unsigned int window_width, const unsigned int window_heght,
  unsigned char arrayMandel[],
  const unsigned int wMandel, const unsigned int hMandel
);

#endif // DRAW_MANDEL_WND_HPP
