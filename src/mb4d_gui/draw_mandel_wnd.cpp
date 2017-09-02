#include "common_nuklear_includes.hpp"
#include "drawing2.hpp"
#include "draw_mandel_wnd.hpp"

void drawMandelWnd(
  struct nk_context *ctx,
  unsigned int *texture,
  unsigned char arrayMandel[],
  const unsigned int wMandel, const unsigned int hMandel
)
{
  int ret_val = nk_begin(
    ctx, "Mandelbulb, pow 8",
    nk_rect(50, 50, wMandel, hMandel),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE | NK_WINDOW_SCALABLE
  );

  if (ret_val == 1) {
    draw_mandelbulb(ctx, texture[0], arrayMandel, wMandel, hMandel);
  }

  nk_end(ctx);
}
