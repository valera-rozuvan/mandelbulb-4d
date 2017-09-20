#include "common_nuklear_includes.hpp"
#include <GL/glew.h>
#include "drawing2.hpp"

void drawMandelbulb(struct nk_context* ctx, unsigned int texture, AppState* appState)
{
  struct nk_command_buffer* canvas;
  struct nk_image myImage;
  struct nk_rect totalSpace;
  float arWnd;
  float oldVal;
  const struct nk_color gridColor = nk_rgba(255, 255, 255, 255);

  canvas = nk_window_get_canvas(ctx);
  totalSpace = nk_window_get_content_region(ctx);
  arWnd = ((float)totalSpace.w) / ((float)totalSpace.h);

  if (arWnd < appState->aspectRatioMandel) {
    oldVal = totalSpace.h;
    totalSpace.h = (totalSpace.w * ((float)appState->hMandel)) / ((float)appState->wMandel);
    totalSpace.y += 0.5 * (oldVal - totalSpace.h);
  } else if (arWnd > appState->aspectRatioMandel) {
    oldVal = totalSpace.w;
    totalSpace.w = (totalSpace.h * ((float)appState->wMandel)) / ((float)appState->hMandel);
    totalSpace.x += 0.5 * (oldVal - totalSpace.w);
  }

  // use a texture
  glBindTexture(GL_TEXTURE_2D, texture);

  // set necessary texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // TODO: Figure out if below is better than above.
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // allocate memory and set texture data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, appState->wMandel, appState->hMandel, 0, GL_RGBA, GL_UNSIGNED_BYTE, appState->arrayMandel);

  glGenerateMipmap(GL_TEXTURE_2D);

  // render texture inside current nuklear window
  myImage = nk_image_id((int)texture);
  nk_draw_image(canvas, totalSpace, &myImage, gridColor);
}
