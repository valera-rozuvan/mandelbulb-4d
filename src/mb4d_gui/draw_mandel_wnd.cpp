#include "common_nuklear_includes.hpp"
#include "drawing2.hpp"
#include "draw_mandel_wnd.hpp"

void drawMandelWnd(
  struct nk_context *ctx,
  unsigned int *texture,
  const unsigned int window_width, const unsigned int window_heght,
  unsigned char arrayMandel[],
  const unsigned int wMandel, const unsigned int hMandel
)
{
  if (nk_begin(
    ctx, "Mandelbulb, pow 8", nk_rect(100, 100, 700, 700),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE | NK_WINDOW_SCALABLE
  ))
  {
    draw_mandelbulb(ctx, texture[0], arrayMandel, wMandel, hMandel);
  }
  nk_end(ctx);
}
