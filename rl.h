/* rl.h - v0.0.0 - a MIT-licensed roguelike building block
  ...tbc
*/


#ifndef RLH
#define RLH

#define RLH_VER "0.0.0"

#ifndef RLH_NO_STDIO
#include <stdio.h>
#endif // RL_NO_STDIO


#ifndef RLH_NO_TYPES
#include <inttypes.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;
#endif

#ifndef RLHDEF
#ifdef RLH_STATIC
#define RLHDEC static
#define RLHDEF static
#else
#define RLHDEC extern
#define RLHDEF
#endif
#endif

typedef enum {
  RLH_ERR       = 0,
  RLH_WARN      = 1,
  RLH_NORM      = 2
} RLH_LOG_LEVEL_E;

typedef enum {
  RLH_STATUS_SUCCESS,
  RLH_STATUS_ERROR,
  RLH_STATUS_QUIT
} RLH_STATUS_E;

// Public API
RLHDEC RLH_STATUS_E rlh_init();

#endif // RLH
// -- END INCLUDE --
// --------------------


// --------------------
// -- BEGIN IMPLEMENTATION --
#ifdef RLH_IMPLEMENTATION

static const char *rlh_log_prefix[] = {
  "\033[0;31m[RLH:ERR]",
  "\033[0;33m[RLH:WARN]",
  "\033[0;0m[RLH:LOG]"
};

#define RLH_LOG(p, f_, ...) printf(("%s " f_), rlh_log_prefix[p], ##__VA_ARGS__)

#ifdef RLH_DEBUG
#define RLH_DBG(f_, ...) printf(("\033[0;34m[DBG] " f_), ##__VA_ARGS__)
#else
#define RLH_DBG(...)
#endif

#ifndef RLH_MALLOC
#define RLH_MALLOC(s)     malloc(s)
#define RLH_REALLOC(p,nz) realloc(p,ns)
#define RLH_FREE(p)       free(p)
#endif

#define RLH_NEW(p, n) ((p) = RLH_MALLOC((n) * sizeof *(p)))

// only api implemented at the moment
#define RLH_API_SDL2

#ifdef RLH_API_SDL2

static SDL_Window   *rlh_sdl_window  = NULL;
static SDL_GLContext rlh_sdl_context = NULL;

RLHDEF RLH_STATUS_E rlh_init() {
  RLH_LOG(RLH_NORM, "Initializing rl.h -- version %s\n", RLH_VER);

  // initialize SDL2
  if (!SDL_Init(SDL_INIT_EVERYTHING)) {
    RLH_LOG(RLH_NORM, "SDL2 initialized\n");
  } else {
    RLH_LOG(RLH_ERR, "Unable to init SDL2: %s\n", SDL_GetError());
    return RLH_STATUS_ERROR;
  }

  // set gl attributes
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0);

  // TODO: let user define title, width, height (with defaults)? 
  u32 width  = 1024;
  u32 height = 1024;
  rlh_sdl_window = SDL_CreateWindow("rl.h",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            width,
                            height,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if (rlh_sdl_window != NULL) {
    RLH_LOG(RLH_NORM, "Window created %ux%u\n", width, height);
  } else {
    RLH_LOG(RLH_ERR, "Unable to open window\n");
    return RLH_STATUS_ERROR;
  }

  // get the gl context
  rlh_sdl_context = SDL_GL_CreateContext(rlh_sdl_window);
  if (rlh_sdl_context != NULL) {
    RLH_LOG(RLH_NORM, "Got OpenGL context\n");
  } else {
    RLH_LOG(RLH_ERR, "Failed creating OpenGL context\n");
    return RLH_STATUS_ERROR;
  }

  // get the gl proc address
  if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    RLH_LOG(RLH_NORM, "Got OpenGL proc address\n");
  } else {
    RLH_LOG(RLH_ERR, "Failed getting OpenGL proc address\n");
    return RLH_STATUS_ERROR;
  }

  SDL_GL_SetSwapInterval(1);

  return RLH_STATUS_SUCCESS;
}

RLHDEF RLH_STATUS_E rlh_render_frame()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // TODO: handle events elsewhere (maybe not)?
  // handle SDL events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
      case SDL_WINDOWEVENT_CLOSE: {
        return RLH_STATUS_QUIT;
        break;
      }

      // input events
      case SDL_KEYDOWN:
      case SDL_KEYUP:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEWHEEL:
      case SDL_TEXTEDITING:
      case SDL_TEXTINPUT:
      case SDL_MOUSEMOTION:
      case SDL_KEYMAPCHANGED: {
        // TODO: handle input
        // TODO: translate input codes to agnostic rlh_input codes
        //       (SDL_SCANCODE_LEFT -> RLH_KEY_LEFT etc)
        // input_event(&event);
        break;
      }
    }

    // handle window events
    if (event.type == SDL_WINDOWEVENT) {
      switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          // TODO: handle window resize
          // ini_set_float(conf, "graphics", "window_width", (float)event.window.data1);
          // ini_set_float(conf, "graphics", "window_height", (float)event.window.data2);
          // render_resize(event.window.data1, event.window.data2);
          break;
        }
      }
      break;
    }
  }

  SDL_GL_SwapWindow(rlh_sdl_window);

  return RLH_STATUS_SUCCESS;
}

RLHDEF RLH_STATUS_E rlh_exit()
{
  RLH_LOG(RLH_NORM, "Cleaning up renderer\n");
  SDL_GL_DeleteContext(rlh_sdl_context);
  SDL_DestroyWindow(rlh_sdl_window);
  SDL_Quit();

  return RLH_STATUS_SUCCESS;
}
#endif // RLH_API_SDL2

#endif // RLH_IMPLEMENTATION
