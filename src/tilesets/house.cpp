#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "tilesets/house.h"
#include "tile_data.h"
#include "datatypes.h"

TilesetHouse::TilesetHouse(void):
  tilenumber(0) {

}

TilesetHouse::load(void) {
  const char *file = "images/tilesets/house.png";
  SDL_Surface *tileset_image = IMG_Load(file);

  if (tileset_image == NULL) {
    std::cerr << "Unable to load " << file << std::endl;
    return;
  }

  SDL_Rect where_in_src;

  // tile 0
  where_in_src.x = 0;
  where_in_src.y = 0;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 1
  where_in_src.x = 24;
  where_in_src.y = 0;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 2
  where_in_src.x = 0;
  where_in_src.y = 24;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 3
  where_in_src.x = 24;
  where_in_src.y = 24;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 4
  where_in_src.x = 48;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 5
  where_in_src.x = 48;
  where_in_src.y = 24;
  where_in_src.w = 16;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 6
  where_in_src.x = 0;
  where_in_src.y = 48;
  where_in_src.w = 24;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 7
  where_in_src.x = 24;
  where_in_src.y = 48;
  where_in_src.w = 24;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 8
  where_in_src.x = 64;
  where_in_src.y = 0;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 9
  where_in_src.x = 88;
  where_in_src.y = 0;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 10
  where_in_src.x = 64;
  where_in_src.y = 24;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 11
  where_in_src.x = 88;
  where_in_src.y = 24;
  where_in_src.w = 24;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 12
  where_in_src.x = 112;
  where_in_src.y = 0;
  where_in_src.w = 16;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 13
  where_in_src.x = 112;
  where_in_src.y = 24;
  where_in_src.w = 16;
  where_in_src.h = 24;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 14
  where_in_src.x = 64;
  where_in_src.y = 48;
  where_in_src.w = 24;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 15
  where_in_src.x = 88;
  where_in_src.y = 48;
  where_in_src.w = 24;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, OBSTACLE);

  // tile 16
  where_in_src.x = 0;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 17
  where_in_src.x = 8;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 18
  where_in_src.x = 0;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 19
  where_in_src.x = 8;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 20
  where_in_src.x = 16;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 21
  where_in_src.x = 24;
  where_in_src.y = 64;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 22
  where_in_src.x = 16;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 23
  where_in_src.x = 24;
  where_in_src.y = 72;
  where_in_src.w = 8;
  where_in_src.h = 8;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

  // tile 24
  where_in_src.x = 32;
  where_in_src.y = 64;
  where_in_src.w = 16;
  where_in_src.h = 16;
  tiles[tile_number++] = new SimpleTile(tileset_image, where_in_src, NO_OBSTACLE);

}
