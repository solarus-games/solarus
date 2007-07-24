/*
 * Tileset generated automatically by the tileset editor.
 */

#include <SDL/SDL.h>
#include "tilesets/TilesetHouse.h"
#include "SimpleTile.h"
#include "AnimatedTile.h"

/**
 * Loads the tileset.
 */
void TilesetHouse::load(void) {
  load_tileset_image("images/tilesets/House.png");
  SDL_Rect position_in_tileset;

  // tile 0
  position_in_tileset.x = 0;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 1
  position_in_tileset.x = 8;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 2
  position_in_tileset.x = 24;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 3
  position_in_tileset.x = 40;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 4
  position_in_tileset.x = 48;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 5
  position_in_tileset.x = 40;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 6
  position_in_tileset.x = 48;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 7
  position_in_tileset.x = 56;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 8
  position_in_tileset.x = 64;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 9
  position_in_tileset.x = 56;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 10
  position_in_tileset.x = 64;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 11
  position_in_tileset.x = 72;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 12
  position_in_tileset.x = 80;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 13
  position_in_tileset.x = 72;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 14
  position_in_tileset.x = 80;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE_NONE);
  // tile 15
  position_in_tileset.x = 88;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 16
  position_in_tileset.x = 104;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 17
  position_in_tileset.x = 120;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 18
  position_in_tileset.x = 136;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 19
  position_in_tileset.x = 0;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 20
  position_in_tileset.x = 8;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 21
  position_in_tileset.x = 0;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 22
  position_in_tileset.x = 8;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 23
  position_in_tileset.x = 16;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 24
  position_in_tileset.x = 24;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 25
  position_in_tileset.x = 16;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 26
  position_in_tileset.x = 24;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 27
  position_in_tileset.x = 32;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 28
  position_in_tileset.x = 40;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 29
  position_in_tileset.x = 32;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 30
  position_in_tileset.x = 40;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 31
  position_in_tileset.x = 0;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 32
  position_in_tileset.x = 16;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 33
  position_in_tileset.x = 0;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 34
  position_in_tileset.x = 16;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 35
  position_in_tileset.x = 32;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 36
  position_in_tileset.x = 48;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 37
  position_in_tileset.x = 32;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 38
  position_in_tileset.x = 48;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 39
  position_in_tileset.x = 64;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 40
  position_in_tileset.x = 80;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 41
  position_in_tileset.x = 64;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 42
  position_in_tileset.x = 80;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 43
  position_in_tileset.x = 0;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 44
  position_in_tileset.x = 8;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 45
  position_in_tileset.x = 0;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 46
  position_in_tileset.x = 8;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 47
  position_in_tileset.x = 16;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 48
  position_in_tileset.x = 24;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 49
  position_in_tileset.x = 16;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 50
  position_in_tileset.x = 24;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 51
  position_in_tileset.x = 32;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 52
  position_in_tileset.x = 40;
  position_in_tileset.y = 64;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 53
  position_in_tileset.x = 32;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 54
  position_in_tileset.x = 40;
  position_in_tileset.y = 72;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 55
  position_in_tileset.x = 0;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 56
  position_in_tileset.x = 8;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 57
  position_in_tileset.x = 0;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 58
  position_in_tileset.x = 8;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 59
  position_in_tileset.x = 16;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 60
  position_in_tileset.x = 24;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 61
  position_in_tileset.x = 16;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 62
  position_in_tileset.x = 24;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 63
  position_in_tileset.x = 32;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 64
  position_in_tileset.x = 40;
  position_in_tileset.y = 80;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 65
  position_in_tileset.x = 32;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 66
  position_in_tileset.x = 40;
  position_in_tileset.y = 88;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 67
  position_in_tileset.x = 16;
  position_in_tileset.y = 96;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 68
  position_in_tileset.x = 24;
  position_in_tileset.y = 96;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 69
  position_in_tileset.x = 16;
  position_in_tileset.y = 104;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
  // tile 70
  position_in_tileset.x = 24;
  position_in_tileset.y = 104;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  tiles[nb_tiles++] = new SimpleTile(position_in_tileset, OBSTACLE);
}
