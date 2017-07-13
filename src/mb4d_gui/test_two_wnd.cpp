#include "common_nuklear_includes.hpp"
#include "drawing.hpp"
#include "test_two_wnd.hpp"

bool show_text = false;

void testTwoWnd(
  struct nk_context *ctx,
  unsigned int *texture
)
{
  if (nk_begin(
    ctx, "Test 2", nk_rect(190, 240, 230, 230),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE
  ))
  {
    draw_line(ctx, texture);

    nk_layout_row_dynamic(ctx, 20, 1);

    if (show_text == true) {
      nk_label(ctx, "Hello, world!", NK_TEXT_LEFT);
    } else {
      nk_label(ctx, "", NK_TEXT_LEFT);
    }

    if (nk_button_label(ctx, "Clear")) {
      show_text = false;
    }

    if (nk_button_label(ctx, "Hello, world!")) {
      show_text = true;
    }
  }
  nk_end(ctx);
}
