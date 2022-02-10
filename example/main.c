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

  rlh_level level = rlh_level_new();

  int w, h, comp;
  u8 *data = stbi_load("tiles.png", &w, &h, &comp, 4);
  free(data);
  if (rlh_add_texture("tiles", w, h, comp, data) == RLH_STATUS_ERROR) {
    printf("Failed adding texture\n");
  }

  /*
    // dynamically resizing tilemap
    tilemap = new_tilemap(depth = 4, default_width = 50, default_height = 50, ...)
    tile = tilemap_get(x, y, z)
    tilemap_set(ENUM_VAL, x, y, z)

    // add a texture, stating the tiles enum range it covers (left to right, top to bottom)
    add_texture(..., MY_TILE_ENUM_FIRST, MY_TILE_ENUM_LAST)

    tilemap_render(origin_x, origin_y)


    // entities
    // using null ptr, can let usr override default struct for entity templates to add more properties?
    template = {
      .name = "goblin",
      .health = 123
    }
    register_template("goblin", template)

    // create a new mob using our previous template
    mob = entity_new(template = "goblin", x, y, override_default = {.name = ..., .health = ...})

    // register the entity with the tilemap so it generates events etc
    entity_bind_map(tilemap)



    // enum containing ALL groups
    enum groups {
      SOLID,
      FLOOR,
      HOLE
    }

    // enum containing ALL tiles
    enum tiles {
      BRICK_WALL_TOP
      BRICK_WALL_BOTTOM
      LAVA
      LAVA_DRY
      WATER
      HOLE
    }

    // group callbacks
    void solid_callback(event) {
      // called when something interacts with a tile in the SOLID group
      // prevents the action that put the entity here from happening
      // thus, prevents the entity walking into the wall
      if (event.before) {
        return event.prevent_default;
      }
    }

    void hole_callback(event) {
      fall_down_hole(...)
    }

    // create a group
    create_group("walls", SOLID, solid_callback)
    create_group("holes", HOLE, hole_callback)

    // then add tiles to the groups
    arr subset[] = {
      BRICK_WALL_TOP,
      BRICK_WALL_BOTTOM,
      ARR_END // null-terminator
    }
    group_add("walls", subset)
    
  */

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
