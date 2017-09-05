#include "common_nuklear_includes.hpp"
#include <GL/glew.h>
#include "drawing2.hpp"

void draw_mandelbulb(
  struct nk_context* ctx,
  unsigned int texture,
  AppState* appState
)
{
  struct nk_command_buffer *canvas;
  struct nk_image myImage;
  struct nk_rect total_space;

  const struct nk_color grid_color = nk_rgba(255, 255, 255, 255);

  canvas = nk_window_get_canvas(ctx);
  total_space = nk_window_get_content_region(ctx);

  // use a texture
  glBindTexture(GL_TEXTURE_2D, texture);

  // set necessary texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /*
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  */

  // allocate memory and set texture data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, appState->wMandel, appState->hMandel, 0, GL_RGBA, GL_UNSIGNED_BYTE, appState->arrayMandel);

  glGenerateMipmap(GL_TEXTURE_2D);

  // render texture inside current nuklear window
  myImage = nk_image_id((int)texture);
  nk_draw_image(canvas, total_space, &myImage, grid_color);
}
