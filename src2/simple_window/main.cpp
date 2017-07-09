/* nuklear - 1.32.0 - public domain */

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

#include <GL/glew.h>
#include <glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "test64bit.cpp"
#include "test_opencl.cpp"
#include "drawing.cpp"
#include "drawing2.cpp"
#include "threads_test.cpp"
#include "mandelbulb.cpp"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

const unsigned int wMandel = 400;
const unsigned int hMandel = 400;

unsigned char arrayMandel[wMandel * hMandel * 4];

static void error_callback(int e, const char *d)
{
  printf("Error %d: %s\n", e, d);
}

int main(void)
{
  test64bit();
  runThreads();
  test_opencl();
  generateMandel(21, arrayMandel, wMandel, hMandel);

  /* Platform */
  static GLFWwindow *win;
  int width = 0, height = 0;
  struct nk_context *ctx;
  struct nk_color background;

  /* GLFW */
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    fprintf(stdout, "[GFLW] failed to init!\n");
    exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
  glfwMakeContextCurrent(win);
  glfwGetWindowSize(win, &width, &height);

  /* OpenGL */
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glewExperimental = 1;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to setup GLEW\n");
    exit(1);
  }

  ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);

  {
    struct nk_font_atlas *atlas;

    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
  }

  bool show_text = false;

  background = nk_rgb(28,48,62);

  while (!glfwWindowShouldClose(win))
  {
    /* Input */
    glfwPollEvents();
    nk_glfw3_new_frame();

    /* GUI */
    if (nk_begin(
      ctx, "Demo", nk_rect(50, 50, 230, 250),
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
      if (nk_combo_begin_color(ctx, background, nk_vec2(nk_widget_width(ctx),400))) {
        nk_layout_row_dynamic(ctx, 120, 1);
        background = nk_color_picker(ctx, background, NK_RGBA);

        nk_layout_row_dynamic(ctx, 25, 1);
        background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
        background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
        background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
        background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);

        nk_combo_end(ctx);
      }
    }
    nk_end(ctx);

    // creating a texture
    unsigned int texture;
    glGenTextures(1, &texture);

    if (nk_begin(
      ctx, "Nuklear", nk_rect(WINDOW_WIDTH/2 - 110 - 300, WINDOW_HEIGHT/2 - 110 - 50, 230, 230),
      NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE
    ))
    {
      nk_layout_row_dynamic(ctx, 20, 1);

      draw_line(ctx, texture);

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

    // creating a texture
    unsigned int texture2;
    glGenTextures(1, &texture2);

    if (nk_begin(
      ctx, "Mandelbulb", nk_rect(WINDOW_WIDTH/2 - 110, WINDOW_HEIGHT/2 - 110, 420, 420),
      NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE
    ))
    {
      draw_mandelbulb(ctx, texture2, arrayMandel, wMandel, hMandel);
    }
    nk_end(ctx);

    /* Draw */
    {
      float bg[4];

      nk_color_fv(bg, background);

      glfwGetWindowSize(win, &width, &height);

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(bg[0], bg[1], bg[2], bg[3]);

      /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
       * with blending, scissor, face culling, depth test and viewport and
       * defaults everything back into a default state.
       * Make sure to either a.) save and restore or b.) reset your own state after
       * rendering the UI.
       */
      nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

      glfwSwapBuffers(win);
    }

    // delete texture
    glDeleteTextures(1, &texture);

    // delete texture
    glDeleteTextures(1, &texture2);
  }

  nk_glfw3_shutdown();
  glfwTerminate();
  test64bit_freeMem();

  return 0;
}
