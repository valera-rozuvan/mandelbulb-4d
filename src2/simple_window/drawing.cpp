struct nk_command_buffer *canvas;
const struct nk_color grid_color = nk_rgb(255, 255, 255);
struct nk_vec2 upperCoord;

void draw_line(struct nk_context* ctx)
{
  canvas = nk_window_get_canvas(ctx);

  upperCoord = nk_window_get_content_region_min(ctx);

  nk_stroke_line(canvas, upperCoord.x + 0, upperCoord.y + 0, upperCoord.x + 10, upperCoord.y + 10, 1.0f, grid_color);
}
