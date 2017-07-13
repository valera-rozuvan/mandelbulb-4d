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
    ctx, "Mandelbulb cross section", nk_rect(window_width/2 - 110, window_heght/2 - 110, 420, 420),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE
  ))
  {
    draw_mandelbulb(ctx, texture[0], arrayMandel, wMandel, hMandel);
  }
  nk_end(ctx);
}
