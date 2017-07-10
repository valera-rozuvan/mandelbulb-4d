#include "drawing.cpp"

bool show_text = false;

void testTwoWnd(
  struct nk_context *ctx,
  unsigned int *texture,
  const unsigned int window_width, const unsigned int window_heght
)
{
    if (nk_begin(
      ctx, "Test 2", nk_rect(window_width/2 - 110 - 300, window_heght/2 - 110 - 50, 230, 230),
      NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE
    ))
    {
      nk_layout_row_dynamic(ctx, 20, 1);

      draw_line(ctx, texture[0]);

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
