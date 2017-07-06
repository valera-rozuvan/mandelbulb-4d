struct nk_command_buffer *canvas;
const struct nk_color grid_color = nk_rgb(255, 255, 255);
struct nk_vec2 upperCoord;

const unsigned int WIDTH_IMG = 80;
const unsigned int HEIGHT_IMG = 80;

unsigned char imageData[WIDTH_IMG * HEIGHT_IMG * 4];

void draw_line(struct nk_context* ctx)
{
  canvas = nk_window_get_canvas(ctx);

  // Draw simple line.
  upperCoord = nk_window_get_content_region_min(ctx);
  nk_stroke_line(canvas, upperCoord.x + 0, upperCoord.y + 0, upperCoord.x + 10, upperCoord.y + 10, 1.0f, grid_color);

  // Create an image. For now all pixels are the same.
  int i = 0;
  for(unsigned int y = 0; y < HEIGHT_IMG; ++y){
      for(unsigned int x = 0; x < WIDTH_IMG; ++x){
        imageData[i] = 174;
        imageData[i + 1] = 79;
        imageData[i + 2] = 206;
        imageData[i + 3] = 255;

        i += 4;
     }
  }

  // creating a texture
  unsigned int texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // set necessary texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // allocate memory and set texture data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH_IMG, HEIGHT_IMG, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

  // TODO: How to render texture inside current nuklear window?

  // delete texture
  glDeleteTextures(1, &texture);

}
