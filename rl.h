/* rl.h - v0.0.0 - a MIT-licensed roguelike building block
  ...tbc
*/

// --------------------
// -- BEGIN INCLUDE --
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
#endif // RLH_NO_TYPES

#ifndef RLH_NO_BOOL
#define TRUE  (1)
#define FALSE (0)
typedef uint8_t bool;
#endif // RLH_NO_BOOL

#ifndef RLH_NO_MATH
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CLAMP(x, l, h) (MAX(l, MIN(x, h)))
#endif // RLH_NO_MATH

#ifndef RLHDEF
#ifdef RLH_STATIC
#define RLHDEC static
#define RLHDEF static
#else
#define RLHDEC extern
#define RLHDEF
#endif // RLH_STATIC
#endif // RLHDEF

typedef enum RLH_LOG_LEVEL_E {
  RLH_ERR       = 0,
  RLH_WARN      = 1,
  RLH_NORM      = 2
} RLH_LOG_LEVEL_E;

typedef enum RLH_STATUS_E {
  RLH_STATUS_SUCCESS,
  RLH_STATUS_ERROR,
  RLH_STATUS_QUIT
} RLH_STATUS_E;

// public API
RLHDEC RLH_STATUS_E rlh_init();
RLHDEC RLH_STATUS_E rlh_render_frame();
RLHDEC RLH_STATUS_E rlh_exit();
RLHDEC bool rlh_keydown(SDL_Scancode k);
RLHDEC bool rlh_keyup(SDL_Scancode k);
RLHDEC bool rlh_mousedown(i32 b);
RLHDEC bool rlh_mouseup(i32 b);
RLHDEC void rlh_mouseposition(i32 *x, i32 *y);
RLHDEC void rlh_setkeyrepeat(bool enable);

#endif // RLH
// -- END INCLUDE --
// --------------------






// --------------------
// -- BEGIN IMPLEMENTATION --



// --------------------
// -- BEGIN IMPLEMENTATION 'HEADER' --
#define RLH_IMPLEMENTATION
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
// -- END IMPLEMENTATION 'HEADER' --
// --------------------



// --------------------
// -- BEGIN IMPLEMENTATION RL.H --
typedef int rlh_level;

typedef struct {
  int depth, width, height;
  int *tiles;
} rlh_level_t;

rlh_level_t **rlh_levels = NULL;
size_t       rlh_level_count = 0;

RLHDEF rlh_level rlh_level_new() {
  if (!rlh_levels) {
    // TODO: variable length? some stb library?
    rlh_levels = malloc(sizeof(rlh_level_t*) * 128);
  }

  RLH_LOG(RLH_NORM, "Generating new empty level\n");
  rlh_levels[rlh_level_count] = malloc(sizeof(rlh_level_t));
  return rlh_level_count++;
}

// -- END IMPLEMENTATION RL.h --
// --------------------



// --------------------
// -- BEGIN IMPLEMENTATION OPENGL --
#ifndef RLH_NO_SDL2
#ifndef RLH_NO_GL

// defs
#define RLH_RENDERER_FLAGS SDL_WINDOW_OPENGL
#define RLH_MAX_TEXTURES   32

// shader sources
const char *rlh_gl_vertex_shader = "\
#version 100 \n\
precision mediump float; \n\
void main() \n\
{ \n\
} \n\
";

const char *rlh_gl_fragment_shader = "\
#version 100 \n\
precision mediump float; \n\
void main() \n\
{ \n\
  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); \n\
} \n\
";

// types
typedef struct {
  float position[3];
  float color[4];
  float uv[2];
} rlh_vertex_t;

typedef struct {
  GLuint id;
  int width, height, components;
  u8 *data, active;
  char name[16];
} rlh_texture_t;

// locals
static SDL_GLContext  rlh_sdl_context    = NULL;
static SDL_Window    *rlh_sdl_window_ptr = NULL;
static GLuint         rlh_shader_program = 0;
static rlh_texture_t  rlh_textures[RLH_MAX_TEXTURES] = { 0 };

RLHDEF RLH_STATUS_E rlh_add_texture(const char *str, int w, int h, int c, u8 *d) {
  // find empty texture slot
  rlh_texture_t *t = NULL;
  for (int i=0; i<RLH_MAX_TEXTURES; i++) {
    if (!rlh_textures[i].active) {
      t = &rlh_textures[i];
      break;
    }
  }

  // no slots left
  if (!t) {
    RLH_LOG(RLH_ERR, "Texture limit (%i) exceeded!\n", RLH_MAX_TEXTURES);
    return RLH_STATUS_ERROR;
  }

  t->width      = w;
  t->height     = h;
  t->components = c;
  t->active     = 1;
  strncpy(t->name, str, 16);

  // generate gl texture
  glGenTextures(1, &t->id);
  glBindTexture(GL_TEXTURE_2D, t->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (c == 4) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
  } else if (c == 3) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, d);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  RLH_LOG(RLH_NORM, "Added new texture: %s\n", t->name);
  return RLH_STATUS_SUCCESS;
}

RLHDEF RLH_STATUS_E rlh_load_shader(GLuint *ret, const char *str, GLenum shdr_type) {
  // create and compile shader
  GLuint shader = glCreateShader(shdr_type);
  glShaderSource(shader, 1, (const GLchar* const*)&str, NULL);
  glCompileShader(shader);

  // check errors
  GLchar compile_log[512]; GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, compile_log);
    RLH_LOG(RLH_ERR, "Failed to compile shader\n%s\n", compile_log);
    return RLH_STATUS_ERROR;
  }

  *ret = shader;

  return RLH_STATUS_SUCCESS;
}

RLHDEF RLH_STATUS_E rlh_init_renderer(SDL_Window *window) {
  rlh_sdl_window_ptr = window;

  // set gl attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0);

  // get the gl context
  rlh_sdl_context = SDL_GL_CreateContext(rlh_sdl_window_ptr);
  if (rlh_sdl_context != NULL) {
    RLH_LOG(RLH_NORM, "Got OpenGL ES 2.0 context\n");
    SDL_GL_MakeCurrent(rlh_sdl_window_ptr, rlh_sdl_context);
  } else {
    RLH_LOG(RLH_ERR, "Failed creating OpenGL ES 2.0 context\n");
    return RLH_STATUS_ERROR;
  }


  // get the gl proc address
  if (gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress)) {
    RLH_LOG(RLH_NORM, "Got OpenGL ES 2.0 proc address\n");
  } else {
    RLH_LOG(RLH_ERR, "Failed getting OpenGL ES 2.0 proc address\n");
    return RLH_STATUS_ERROR;
  }

  SDL_GL_SetSwapInterval(1);

  // load the shaders
  GLuint fshader, vshader;
  if (rlh_load_shader(&vshader, rlh_gl_vertex_shader, GL_FRAGMENT_SHADER) == RLH_STATUS_ERROR ||
      rlh_load_shader(&fshader, rlh_gl_fragment_shader, GL_FRAGMENT_SHADER) == RLH_STATUS_ERROR) {
    return RLH_STATUS_ERROR;
  } else {
    RLH_LOG(RLH_NORM, "Compiled shaders\n");
  }

  // create the shader program
  rlh_shader_program = glCreateProgram();
  glAttachShader(rlh_shader_program, vshader);
  glAttachShader(rlh_shader_program, fshader);
  glLinkProgram(rlh_shader_program);
  glUseProgram(rlh_shader_program);

  GLuint rlh_vertices_vbo;
  GLuint rlh_indices_vbo;
  
  // TODO: tilemap rendering
  // ideas: have a unit-tilemap, a square vertex field where each 'tile' has the size of 1
  //        each time a tilemap is rendered, the vertex field is scaled up to match the correct tile size
  //        and the uv coords are updated accordingly

  glGenBuffers(1, &rlh_vertices_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, rlh_vertices_vbo);
  // glBufferData .. vertices
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &rlh_indices_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rlh_indices_vbo);
  // glBufferData .. indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // render
  // glUseProgram(program);
  // glBindBuffer(GL_ARRAY_BUFFER, rlh_tilemap_vbo);
  // GLint pos = glGetAttribLocation(sp, "position"); (do this once at init?)
  // glEnableVertexAttribArray(pos)
  // glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  // repeat above 3 steps for color etc
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
  // glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

  // typedef struct {
  //   GLuint vao, vbo, shader;
  //   u32 tile_count;
  //   float    *vertices;
  //   u32 vertex_count;
  //   u32 width,  height;
  //   u32 rwidth, rheight;
  //   mat4x4   transform;
  //   texture_t *texture;
  //   float *tile_uv;
  //   int initialized;
  // } render_tilemap_t;

  return RLH_STATUS_SUCCESS;
}

RLHDEF void rlh_renderer_begin() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

RLHDEF void rlh_renderer_end() {
  SDL_GL_SwapWindow(rlh_sdl_window_ptr);
}

#endif // RLH_NO_GL
// -- END OPENGL IMPLEMENTATION --
// --------------------



// --------------------
// -- BEGIN IMPLEMENTATION SDL2 --
#define RLH_NUM_BUTTONS 16

// private api
RLHDEC RLH_STATUS_E rlh_input(SDL_Event *event);
RLHDEF void rlh_update_input();

// locals
static SDL_Window   *rlh_sdl_window  = NULL;
static u8            rlh_keys_down[SDL_NUM_SCANCODES] = { FALSE };
static u8            rlh_buttons_down[RLH_NUM_BUTTONS];
static i32           rlh_mouse_position[2];
static bool          rlh_key_repeat = TRUE;

void (*rlh_ptr_keypressed)(SDL_Scancode, bool) = NULL;
void (*rlh_ptr_keyreleased)(SDL_Scancode)      = NULL;
void (*rlh_ptr_mousepressed)(i32)              = NULL;
void (*rlh_ptr_mousereleased)(i32)             = NULL;
void (*rlh_ptr_mousewheel)(i32, i32)           = NULL;
void (*rlh_ptr_mousemotion)(i32,i32)           = NULL;

RLHDEF void rlh_void() {};

RLHDEF RLH_STATUS_E rlh_init() {
  RLH_LOG(RLH_NORM, "Initializing rl.h -- version %s\n", RLH_VER);

  // initialize SDL2
  if (!SDL_Init(SDL_INIT_EVERYTHING)) {
    RLH_LOG(RLH_NORM, "SDL2 initialized\n");
  } else {
    RLH_LOG(RLH_ERR, "Unable to init SDL2: %s\n", SDL_GetError());
    return RLH_STATUS_ERROR;
  }

  // TODO: let user define title, width, height (with defaults)? 
  u32 width  = 1024;
  u32 height = 1024;
  rlh_sdl_window = SDL_CreateWindow("rl.h",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            width,
                            height,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | RLH_RENDERER_FLAGS);

  if (rlh_sdl_window != NULL) {
    RLH_LOG(RLH_NORM, "Window created %ux%u\n", width, height);
  } else {
    RLH_LOG(RLH_ERR, "Unable to open window\n");
    return RLH_STATUS_ERROR;
  }

  // init the rendering backend
  if (rlh_init_renderer(rlh_sdl_window) == RLH_STATUS_ERROR) {
    RLH_LOG(RLH_ERR, "Failed to initialize renderer\n");
    return RLH_STATUS_ERROR;
  }

  return RLH_STATUS_SUCCESS;
}

RLHDEF void rlh_update_input() {
  if (SDL_GetRelativeMouseMode()) {
    SDL_GetRelativeMouseState(&rlh_mouse_position[0], &rlh_mouse_position[1]);
  } else {
    SDL_GetMouseState(&rlh_mouse_position[0], &rlh_mouse_position[1]);
  }
}

RLHDEF RLH_STATUS_E rlh_render_frame() {
  // TODO: move this to game loop
  rlh_update_input();

  rlh_renderer_begin();

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
        // TODO: translate input codes to agnostic rlh_input codes
        //       (SDL_SCANCODE_LEFT -> RLH_KEY_LEFT etc)
        rlh_input(&event);
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

  rlh_renderer_end();

  return RLH_STATUS_SUCCESS;
}

RLHDEF RLH_STATUS_E rlh_input(SDL_Event *event) {
  switch (event->type) {
    // keyboard input
    case SDL_KEYDOWN: {
      rlh_keys_down[event->key.keysym.scancode] = TRUE;
      if (!event->key.repeat || (event->key.repeat && rlh_key_repeat)) {
        rlh_ptr_keypressed ? rlh_ptr_keypressed(event->key.keysym.scancode, event->key.repeat) : rlh_void();
      }
      break;
    }
    case SDL_KEYUP: {
      rlh_keys_down[event->key.keysym.scancode] = FALSE;
      rlh_ptr_keyreleased ? rlh_ptr_keyreleased(event->key.keysym.scancode) : rlh_void();
      break;
    }

    // mouse input
    case SDL_MOUSEBUTTONDOWN: {
      rlh_buttons_down[event->button.button] = TRUE;
      rlh_ptr_mousepressed ? rlh_ptr_mousepressed(event->button.button) : rlh_void();
      break;
    }
    case SDL_MOUSEBUTTONUP: {
      rlh_buttons_down[event->button.button] = FALSE;
      rlh_ptr_mousereleased ? rlh_ptr_mousereleased(event->button.button) : rlh_void();
      break;
    }
    case SDL_MOUSEWHEEL: {
      rlh_ptr_mousewheel ? rlh_ptr_mousewheel(event->wheel.x, event->wheel.y) : rlh_void();
      break;
    }
    case SDL_MOUSEMOTION: {
      // TODO: change this to motion.x/y based on user-toggle?
      rlh_ptr_mousemotion ? rlh_ptr_mousemotion(event->motion.xrel, event->motion.yrel) : rlh_void();
      break;
    }
  }

  return RLH_STATUS_SUCCESS;
}

RLHDEF bool rlh_keydown(SDL_Scancode k) {
  return rlh_keys_down[CLAMP(k, 0, SDL_NUM_SCANCODES)] == TRUE;
}

RLHDEF bool rlh_keyup(SDL_Scancode k) {
  return rlh_keys_down[CLAMP(k, 0, SDL_NUM_SCANCODES)] == FALSE;
}

RLHDEF bool rlh_mousedown(i32 b) {
  return rlh_buttons_down[CLAMP(b, 0, RLH_NUM_BUTTONS)] == TRUE;
}

RLHDEF bool rlh_mouseup(i32 b) {
  return rlh_buttons_down[CLAMP(b, 0, RLH_NUM_BUTTONS)] == FALSE;
}

RLHDEF void rlh_mouseposition(i32 *x, i32 *y) {
  *x = rlh_mouse_position[0];
  *y = rlh_mouse_position[1];
}

RLHDEF void rlh_setkeyrepeat(bool enable) {
  rlh_key_repeat = enable;
}

RLHDEF RLH_STATUS_E rlh_exit() {
  RLH_LOG(RLH_NORM, "Cleaning up renderer\n");
  SDL_GL_DeleteContext(rlh_sdl_context);
  SDL_DestroyWindow(rlh_sdl_window);
  SDL_Quit();

  return RLH_STATUS_SUCCESS;
}
#endif // RLH_NO_SDL2
// -- END SDL2 IMPLEMENTATION --
// --------------------



#endif // RLH_IMPLEMENTATION
// -- END IMPLEMENTATION --
// --------------------
