#include "common_nuklear_includes.hpp"
#include <GL/glew.h>
#include "drawing.hpp"

void drawLine(struct nk_context *ctx, unsigned int *texture)
{
  struct nk_command_buffer* canvas;
  const struct nk_color gridColor = nk_rgba(255, 255, 255, 255);
  const struct nk_color lineColor = nk_rgba(123, 44, 189, 255);
  struct nk_vec2 upperCoord;

  const unsigned int WIDTH_IMG = 120;
  const unsigned int HEIGHT_IMG = 120;

  int i;

  unsigned int x;
  unsigned int y;

  unsigned char imageData[WIDTH_IMG * HEIGHT_IMG * 4];

  struct nk_image myImage;
  struct nk_rect totalSpace;

  canvas = nk_window_get_canvas(ctx);
  totalSpace = nk_window_get_content_region(ctx);

  // Create an image. For now all pixels are the same.
  i = 0;
  for (y = 0; y < HEIGHT_IMG; ++y) {
    for (x = 0; x < WIDTH_IMG; ++x) {
      imageData[i] = 174;
      imageData[i + 1] = 79;
      imageData[i + 2] = 206;
      imageData[i + 3] = 255;

      i += 4;
    }
  }

  // use a texture
  glBindTexture(GL_TEXTURE_2D, texture[0]);

  // set necessary texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // TODO: Figure out if below is better than above.
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // allocate memory and set texture data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH_IMG, HEIGHT_IMG, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

  glGenerateMipmap(GL_TEXTURE_2D);

  // render texture inside current nuklear window
  myImage = nk_image_id((int)texture[0]);
  nk_draw_image(canvas, totalSpace, &myImage, gridColor);


  // Draw simple line.
  upperCoord = nk_window_get_content_region_min(ctx);
  nk_stroke_line(canvas, upperCoord.x + 0, upperCoord.y + 0, upperCoord.x + 100, upperCoord.y + 100, 1.0f, lineColor);
}
