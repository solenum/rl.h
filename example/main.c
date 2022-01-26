#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "glad.h"

#define RLH_DEBUG
#define RLH_API_SDL2
#define RLH_IMPLEMENTATION
#include "rl.h"

int main(int argc, char **argv)
{
  if (rlh_init() == RLH_STATUS_SUCCESS) {
    printf("Initialized rl.h library\n");
  } else {
    printf("rlh_init failed\n");
    return EXIT_FAILURE;
  }

  for (;;) {
    
    // render a frame
    if (rlh_render_frame() == RLH_STATUS_QUIT) {
      break;
    }
  }

  printf("Exiting game\n");
  rlh_exit();

  return EXIT_SUCCESS;
}
