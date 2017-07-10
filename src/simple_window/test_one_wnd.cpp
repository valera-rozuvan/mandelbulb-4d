void testOneWnd(struct nk_context *ctx, struct nk_color *background)
{
  if (nk_begin(
    ctx, "Test 1", nk_rect(50, 50, 230, 250),
    NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE
  ))
  {
    enum {EASY, HARD};

    static int op = EASY;
    static int property = 20;

    nk_layout_row_static(ctx, 30, 80, 1);
    if (nk_button_label(ctx, "button")) {
      fprintf(stdout, "button pressed\n");
    }

    nk_layout_row_dynamic(ctx, 30, 2);
    if (nk_option_label(ctx, "easy", op == EASY)) {
      op = EASY;
    }
    if (nk_option_label(ctx, "hard", op == HARD)) {
      op = HARD;
    }

    nk_layout_row_dynamic(ctx, 25, 1);
    nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_label(ctx, "background:", NK_TEXT_LEFT);

    nk_layout_row_dynamic(ctx, 25, 1);
    if (nk_combo_begin_color(ctx, background[0], nk_vec2(nk_widget_width(ctx),400))) {
      nk_layout_row_dynamic(ctx, 120, 1);
      background[0] = nk_color_picker(ctx, background[0], NK_RGBA);

      nk_layout_row_dynamic(ctx, 25, 1);
      background[0].r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background[0].r, 255, 1,1);
      background[0].g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background[0].g, 255, 1,1);
      background[0].b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background[0].b, 255, 1,1);
      background[0].a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background[0].a, 255, 1,1);

      nk_combo_end(ctx);
    }
  }
  nk_end(ctx);
}
