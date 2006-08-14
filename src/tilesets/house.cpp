#include <SDL/SDL.h>
#include "tilesets/house.h"
#include "tile_data.h"
#include "datatypes.h"

void TilesetHouse::load(void) {
  load_tileset_image("images/tilesets/house.png");
  SDL_Rect where_in_src;

  // tile 0
  where_in_src.x = 0;
  where_in_src.y = 0;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 1
  where_in_src.x = 0;
  where_in_src.y = 8;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 2
  where_in_src.x = 8;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 3
  where_in_src.x = 24;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 4
  where_in_src.x = 40;
  where_in_src.y = 0;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 5
  where_in_src.x = 48;
  where_in_src.y = 0;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 6
  where_in_src.x = 40;
  where_in_src.y = 8;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 7
  where_in_src.x = 48;
  where_in_src.y = 8;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 8
  where_in_src.x = 56;
  where_in_src.y = 0;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 9
  where_in_src.x = 64;
  where_in_src.y = 0;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 10
  where_in_src.x = 56;
  where_in_src.y = 8;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 11
  where_in_src.x = 64;
  where_in_src.y = 8;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 12
  where_in_src.x = 72;
  where_in_src.y = 0;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 13
  where_in_src.x = 80;
  where_in_src.y = 0;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 14
  where_in_src.x = 72;
  where_in_src.y = 8;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 15
  where_in_src.x = 80;
  where_in_src.y = 8;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 16
  where_in_src.x = 88;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 17
  where_in_src.x = 104;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 18
  where_in_src.x = 120;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 19
  where_in_src.x = 136;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 20
  where_in_src.x = 0;
  where_in_src.y = 16;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 21
  where_in_src.x = 8;
  where_in_src.y = 16;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 22
  where_in_src.x = 0;
  where_in_src.y = 24;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 23
  where_in_src.x = 8;
  where_in_src.y = 24;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 24
  where_in_src.x = 16;
  where_in_src.y = 16;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 25
  where_in_src.x = 24;
  where_in_src.y = 16;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 26
  where_in_src.x = 16;
  where_in_src.y = 24;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 27
  where_in_src.x = 24;
  where_in_src.y = 24;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 28
  where_in_src.x = 32;
  where_in_src.y = 16;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 29
  where_in_src.x = 40;
  where_in_src.y = 16;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 30
  where_in_src.x = 32;
  where_in_src.y = 24;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 31
  where_in_src.x = 40;
  where_in_src.y = 24;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 32
  where_in_src.x = 0;
  where_in_src.y = 32;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 33
  where_in_src.x = 16;
  where_in_src.y = 32;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 34
  where_in_src.x = 0;
  where_in_src.y = 48;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 35
  where_in_src.x = 16;
  where_in_src.y = 48;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 36
  where_in_src.x = 32;
  where_in_src.y = 32;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 37
  where_in_src.x = 48;
  where_in_src.y = 32;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 38
  where_in_src.x = 32;
  where_in_src.y = 48;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 39
  where_in_src.x = 48;
  where_in_src.y = 48;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 40
  where_in_src.x = 64;
  where_in_src.y = 32;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 41
  where_in_src.x = 80;
  where_in_src.y = 32;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 42
  where_in_src.x = 64;
  where_in_src.y = 48;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 43
  where_in_src.x = 80;
  where_in_src.y = 48;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 44
  where_in_src.x = 0;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 45
  where_in_src.x = 8;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 46
  where_in_src.x = 0;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 47
  where_in_src.x = 8;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 48
  where_in_src.x = 16;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 49
  where_in_src.x = 24;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 50
  where_in_src.x = 16;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 51
  where_in_src.x = 24;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 52
  where_in_src.x = 32;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 53
  where_in_src.x = 40;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 54
  where_in_src.x = 32;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 55
  where_in_src.x = 40;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 56
  where_in_src.x = 0;
  where_in_src.y = 80;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 57
  where_in_src.x = 8;
  where_in_src.y = 80;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 58
  where_in_src.x = 0;
  where_in_src.y = 88;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 59
  where_in_src.x = 8;
  where_in_src.y = 88;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 60
  where_in_src.x = 16;
  where_in_src.y = 80;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 61
  where_in_src.x = 24;
  where_in_src.y = 80;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 62
  where_in_src.x = 16;
  where_in_src.y = 88;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 63
  where_in_src.x = 24;
  where_in_src.y = 88;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 64
  where_in_src.x = 32;
  where_in_src.y = 80;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 65
  where_in_src.x = 40;
  where_in_src.y = 80;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 66
  where_in_src.x = 32;
  where_in_src.y = 88;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 67
  where_in_src.x = 40;
  where_in_src.y = 88;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new ExtensibleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 68
  where_in_src.x = 16;
  where_in_src.y = 96;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 69
  where_in_src.x = 24;
  where_in_src.y = 96;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 70
  where_in_src.x = 16;
  where_in_src.y = 104;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 71
  where_in_src.x = 24;
  where_in_src.y = 104;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);
}
