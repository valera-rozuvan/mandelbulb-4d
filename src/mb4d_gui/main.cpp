#include <string.h>
#include <time.h>

#define NK_IMPLEMENTATION
#include "common_nuklear_includes.hpp"
#include <GL/glew.h>
#include <glfw3.h>
#include "nuklear_glfw_gl3.h"

#include "test64bit.hpp"
#include "test_opencl.hpp"
#include "threads_test.hpp"
#include "mcamera.hpp"
#include "generate_fractal.hpp"

#include "app_state.hpp"

#include "parallel.hpp"

#include "test_one_wnd.hpp"
#include "test_two_wnd.hpp"
#include "draw_mandel_wnd.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

static void error_callback(int e, const char* d)
{
  fprintf(stderr, "Error %d: %s\n", e, d);
}

int main(void)
{
  int ret = 0;
  double time_spent = 0.0;
  unsigned int texture2;
  float bg[4];

  AppState* appState = NULL;

  /* Nuklear window stuff */
  static GLFWwindow* win;
  int width = 0, height = 0;
  struct nk_context* ctx;
  struct nk_color background;

  test64bit_allocMem();
  test64bit_runTest();
  test64bit_freeMem();

  test_threads();

  ret = test_opencl();
  if (ret == 0) {
    fprintf(stdout, "OpenCL test passed.\n");
  } else {
    fprintf(stderr, "OpenCL test failed.\n");
  }

  appState = new AppState(1600, 1200);

  appState->parallel->createStartThread();

  // fprintf(stdout, "Generating fractal image, please wait ...\n");
  // clock_t begin = clock();
  // generate_fractal(appState);
  // clock_t end = clock();
  // time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  // fprintf(stdout, "Done. Elapsed time: %f seconds.\n", time_spent);

  /* GLFW */
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    fprintf(stdout, "ERROR: GFLW failed to initialize!\n");
    exit(1);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  #ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "m4d", NULL, NULL);
  glfwMakeContextCurrent(win);
  glfwGetWindowSize(win, &width, &height);

  /* OpenGL */
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glewExperimental = 1;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "ERROR: Failed to setup GLEW!\n");
    exit(1);
  }

  ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);

  {
    struct nk_font_atlas *atlas;

    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
  }

  background = nk_rgb(133, 149, 175);

  while (!glfwWindowShouldClose(win))
  {
    /* Input */
    glfwPollEvents();
    nk_glfw3_new_frame();


    /* Windows */
    // testOneWnd(ctx, &background);

    // unsigned int texture;
    // glGenTextures(1, &texture);
    // testTwoWnd(ctx, &texture);

    glGenTextures(1, &texture2);
    drawMandelWnd(ctx, &texture2, appState);


    /* Draw */
    {
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


    /* Clean up */
    // glDeleteTextures(1, &texture);
    glDeleteTextures(1, &texture2);
  }

  nk_glfw3_shutdown();
  glfwTerminate();

  delete appState;

  return 0;
}
