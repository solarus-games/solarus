#include <SDL/SDL.h>
#include "tilesets/house.h"
#include "simple_tile.h"
#include "extensible_tile.h"
#include "animated_tile.h"

void TilesetHouse::load(void) {
  load_tileset_image("images/tilesets/house.png");
  SDL_Rect where_in_tileset;

  // tile 0
  where_in_tileset.x = 0;
  where_in_tileset.y = 0;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 1
  where_in_tileset.x = 0;
  where_in_tileset.y = 8;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 2
  where_in_tileset.x = 8;
  where_in_tileset.y = 0;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 3
  where_in_tileset.x = 24;
  where_in_tileset.y = 0;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 4
  where_in_tileset.x = 40;
  where_in_tileset.y = 0;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 5
  where_in_tileset.x = 48;
  where_in_tileset.y = 0;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 6
  where_in_tileset.x = 40;
  where_in_tileset.y = 8;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 7
  where_in_tileset.x = 48;
  where_in_tileset.y = 8;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 8
  where_in_tileset.x = 56;
  where_in_tileset.y = 0;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 9
  where_in_tileset.x = 64;
  where_in_tileset.y = 0;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 10
  where_in_tileset.x = 56;
  where_in_tileset.y = 8;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 11
  where_in_tileset.x = 64;
  where_in_tileset.y = 8;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, NO_OBSTACLE);

  // tile 12
  where_in_tileset.x = 72;
  where_in_tileset.y = 0;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 13
  where_in_tileset.x = 80;
  where_in_tileset.y = 0;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 14
  where_in_tileset.x = 72;
  where_in_tileset.y = 8;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 15
  where_in_tileset.x = 80;
  where_in_tileset.y = 8;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, NO_OBSTACLE);

  // tile 16
  where_in_tileset.x = 88;
  where_in_tileset.y = 0;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 17
  where_in_tileset.x = 104;
  where_in_tileset.y = 0;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 18
  where_in_tileset.x = 120;
  where_in_tileset.y = 0;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 19
  where_in_tileset.x = 136;
  where_in_tileset.y = 0;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 20
  where_in_tileset.x = 0;
  where_in_tileset.y = 16;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 21
  where_in_tileset.x = 8;
  where_in_tileset.y = 16;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 22
  where_in_tileset.x = 0;
  where_in_tileset.y = 24;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 23
  where_in_tileset.x = 8;
  where_in_tileset.y = 24;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 24
  where_in_tileset.x = 16;
  where_in_tileset.y = 16;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 25
  where_in_tileset.x = 24;
  where_in_tileset.y = 16;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 26
  where_in_tileset.x = 16;
  where_in_tileset.y = 24;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 27
  where_in_tileset.x = 24;
  where_in_tileset.y = 24;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 28
  where_in_tileset.x = 32;
  where_in_tileset.y = 16;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 29
  where_in_tileset.x = 40;
  where_in_tileset.y = 16;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 30
  where_in_tileset.x = 32;
  where_in_tileset.y = 24;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 31
  where_in_tileset.x = 40;
  where_in_tileset.y = 24;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 32
  where_in_tileset.x = 0;
  where_in_tileset.y = 32;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 33
  where_in_tileset.x = 16;
  where_in_tileset.y = 32;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 34
  where_in_tileset.x = 0;
  where_in_tileset.y = 48;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 35
  where_in_tileset.x = 16;
  where_in_tileset.y = 48;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 36
  where_in_tileset.x = 32;
  where_in_tileset.y = 32;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 37
  where_in_tileset.x = 48;
  where_in_tileset.y = 32;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 38
  where_in_tileset.x = 32;
  where_in_tileset.y = 48;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 39
  where_in_tileset.x = 48;
  where_in_tileset.y = 48;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 40
  where_in_tileset.x = 64;
  where_in_tileset.y = 32;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 41
  where_in_tileset.x = 80;
  where_in_tileset.y = 32;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 42
  where_in_tileset.x = 64;
  where_in_tileset.y = 48;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 43
  where_in_tileset.x = 80;
  where_in_tileset.y = 48;
  where_in_tileset.w = 16;
  where_in_tileset.h = 16;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 44
  where_in_tileset.x = 0;
  where_in_tileset.y = 64;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 45
  where_in_tileset.x = 8;
  where_in_tileset.y = 64;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 46
  where_in_tileset.x = 0;
  where_in_tileset.y = 72;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 47
  where_in_tileset.x = 8;
  where_in_tileset.y = 72;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 48
  where_in_tileset.x = 16;
  where_in_tileset.y = 64;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 49
  where_in_tileset.x = 24;
  where_in_tileset.y = 64;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 50
  where_in_tileset.x = 16;
  where_in_tileset.y = 72;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 51
  where_in_tileset.x = 24;
  where_in_tileset.y = 72;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 52
  where_in_tileset.x = 32;
  where_in_tileset.y = 64;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 53
  where_in_tileset.x = 40;
  where_in_tileset.y = 64;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 54
  where_in_tileset.x = 32;
  where_in_tileset.y = 72;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 55
  where_in_tileset.x = 40;
  where_in_tileset.y = 72;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 56
  where_in_tileset.x = 0;
  where_in_tileset.y = 80;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 57
  where_in_tileset.x = 8;
  where_in_tileset.y = 80;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 58
  where_in_tileset.x = 0;
  where_in_tileset.y = 88;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 59
  where_in_tileset.x = 8;
  where_in_tileset.y = 88;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 60
  where_in_tileset.x = 16;
  where_in_tileset.y = 80;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 61
  where_in_tileset.x = 24;
  where_in_tileset.y = 80;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 62
  where_in_tileset.x = 16;
  where_in_tileset.y = 88;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 63
  where_in_tileset.x = 24;
  where_in_tileset.y = 88;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 64
  where_in_tileset.x = 32;
  where_in_tileset.y = 80;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 65
  where_in_tileset.x = 40;
  where_in_tileset.y = 80;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 66
  where_in_tileset.x = 32;
  where_in_tileset.y = 88;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 67
  where_in_tileset.x = 40;
  where_in_tileset.y = 88;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new ExtensibleTile(where_in_tileset, OBSTACLE);

  // tile 68
  where_in_tileset.x = 16;
  where_in_tileset.y = 96;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 69
  where_in_tileset.x = 24;
  where_in_tileset.y = 96;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 70
  where_in_tileset.x = 16;
  where_in_tileset.y = 104;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);

  // tile 71
  where_in_tileset.x = 24;
  where_in_tileset.y = 104;
  where_in_tileset.w = 8;
  where_in_tileset.h = 8;
  tiles[tile_number++] = new SimpleTile(where_in_tileset, OBSTACLE);
}
