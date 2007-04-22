/**
 * The map of Link's house.
 * For now it is coded manually, but this C++ file will be generated
 * by the map editor soon.
 */

#include <SDL/SDL.h>
#include "tilesets/house.h"
#include "simple_tile_image.h"
#include "animated_tile_image.h"

/**
 * Loads the map.
 */
void TilesetHouse::load(void) {
  load_tileset_image("images/tilesets/house.png");
  SDL_Rect position_in_tileset;
  //   SDL_Rect positions_in_tileset[3]; // for animated tiles

  // tile 0
  position_in_tileset.x = 0;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 1
  position_in_tileset.x = 0;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 2
  position_in_tileset.x = 8;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 3
  position_in_tileset.x = 24;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 4
  position_in_tileset.x = 40;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 5
  position_in_tileset.x = 48;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 6
  position_in_tileset.x = 40;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 7
  position_in_tileset.x = 48;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 8
  position_in_tileset.x = 56;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 9
  position_in_tileset.x = 64;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 10
  position_in_tileset.x = 56;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 11
  position_in_tileset.x = 64;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 12
  position_in_tileset.x = 72;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 13
  position_in_tileset.x = 80;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 14
  position_in_tileset.x = 72;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 15
  position_in_tileset.x = 80;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, NO_OBSTACLE);

  // tile 16
  position_in_tileset.x = 88;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 17
  position_in_tileset.x = 104;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 18
  position_in_tileset.x = 120;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 19
  position_in_tileset.x = 136;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 20
  position_in_tileset.x = 0;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 21
  position_in_tileset.x = 8;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 22
  position_in_tileset.x = 0;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 23
  position_in_tileset.x = 8;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 24
  position_in_tileset.x = 16;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 25
  position_in_tileset.x = 24;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 26
  position_in_tileset.x = 16;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 27
  position_in_tileset.x = 24;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 28
  position_in_tileset.x = 32;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 29
  position_in_tileset.x = 40;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 30
  position_in_tileset.x = 32;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 31
  position_in_tileset.x = 40;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 32
  position_in_tileset.x = 0;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 33
  position_in_tileset.x = 16;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 34
  position_in_tileset.x = 0;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 35
  position_in_tileset.x = 16;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 36
  position_in_tileset.x = 32;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 37
  position_in_tileset.x = 48;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 38
  position_in_tileset.x = 32;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 39
  position_in_tileset.x = 48;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 40
  position_in_tileset.x = 64;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 41
  position_in_tileset.x = 80;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 42
  position_in_tileset.x = 64;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 43
  position_in_tileset.x = 80;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 44
  position_in_tileset.x = 0;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 45
  position_in_tileset.x = 8;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 46
  position_in_tileset.x = 0;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 47
  position_in_tileset.x = 8;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 48
  position_in_tileset.x = 16;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 49
  position_in_tileset.x = 24;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 50
  position_in_tileset.x = 16;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 51
  position_in_tileset.x = 24;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 52
  position_in_tileset.x = 32;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 53
  position_in_tileset.x = 40;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 54
  position_in_tileset.x = 32;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 55
  position_in_tileset.x = 40;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 56
  position_in_tileset.x = 0;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 57
  position_in_tileset.x = 8;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 58
  position_in_tileset.x = 0;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 59
  position_in_tileset.x = 8;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 60
  position_in_tileset.x = 16;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 61
  position_in_tileset.x = 24;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 62
  position_in_tileset.x = 16;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 63
  position_in_tileset.x = 24;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 64
  position_in_tileset.x = 32;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 65
  position_in_tileset.x = 40;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 66
  position_in_tileset.x = 32;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 67
  position_in_tileset.x = 40;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 68
  position_in_tileset.x = 16;
  position_in_tileset.y = 96;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 69
  position_in_tileset.x = 24;
  position_in_tileset.y = 96;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 70
  position_in_tileset.x = 16;
  position_in_tileset.y = 104;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 71
  position_in_tileset.x = 24;
  position_in_tileset.y = 104;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTileImage(position_in_tileset, OBSTACLE);

  // tile 72 (test for an animated tile)
  /*
  positions_in_tileset[0].x = 8;
  positions_in_tileset[0].y = 0;
  positions_in_tileset[0].w = 16;
  positions_in_tileset[0].h = 16;

  positions_in_tileset[1].x = 24;
  positions_in_tileset[1].y = 0;
  positions_in_tileset[1].w = 16;
  positions_in_tileset[1].h = 16;

  positions_in_tileset[2].x = 40;
  positions_in_tileset[2].y = 0;
  positions_in_tileset[2].w = 16;
  positions_in_tileset[2].h = 16;

  tiles[nb_tiles++] = new AnimatedTileImage(positions_in_tileset, ANIMATION_SEQUENCE_0121, NO_OBSTACLE);
  */
}
