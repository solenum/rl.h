#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "lib/glad.h"

#define RLH_DEBUG
#define RLH_API_SDL2
#define RLH_IMPLEMENTATION
#include "rl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

void keydown(SDL_Scancode key, bool repeat)
{
  // printf("Key pressed %i %i\n", key, repeat);
}

void keyup(SDL_Scancode key)
{
  // printf("Key released %i\n", key);
}

void mousedown(i32 button)
{
  // printf("Mouse pressed %i\n", button);
}

void mouseup(i32 button)
{
  // printf("Mouse released %i\n", button);
}

void mousewheel(i32 dx, i32 dy)
{
  // printf("Mouse wheel %i %i\n", dx, dy);
}

void mousemotion(i32 dx, i32 dy)
{
  // printf("Mouse motion %i %i\n", dx, dy);
}

int main(int argc, char **argv)
{
  // set input callbacks
  rlh_ptr_keypressed    = keydown;
  rlh_ptr_keyreleased   = keyup;
  rlh_ptr_mousepressed  = mousedown;
  rlh_ptr_mousereleased = mouseup;
  rlh_ptr_mousewheel    = mousewheel;
  rlh_ptr_mousemotion   = mousemotion;

  // init rl.h
  if (rlh_init() == RLH_STATUS_SUCCESS) {
    printf("Initialized rl.h library\n");
  } else {
    printf("rlh_init failed\n");
    return EXIT_FAILURE;
  }

  int w, h, comp;
  u8 *data = stbi_load("tiles.png", &w, &h, &comp, 4);
  free(data);
  if (rlh_add_texture("tiles", w, h, comp, data) == RLH_STATUS_ERROR) {
    printf("Failed adding texture\n");
  }

  // do game & render loop
  for (;;) {
    // render a frame
    if (rlh_render_frame() == RLH_STATUS_QUIT) {
      break;
    }
  }

  // cleanup and exit
  printf("Exiting game\n");
  rlh_exit();

  return EXIT_SUCCESS;
}
