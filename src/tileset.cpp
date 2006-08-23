#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "tileset.h"

Tileset::Tileset(void):
extensible_tiles(new DynamicArray<ExtensibleTile*>()), tile_number(0) {

}

Tileset::~Tileset(void) {
  if (is_loaded()) {
    unload();
  }
}

void Tileset::load_tileset_image(const char *file) {
  tileset_image = IMG_Load(file);
  
  if (tileset_image == NULL) {
    std::cerr << "Unable to load0 " << file << std::endl;
  }
}

void Tileset::unload(void) {
  int i;

  for (i = 0; i < tile_number; i++) {
    delete tiles[i];
  }

  for (i = 0; i < extensible_tiles->get_size(); i++) {
    delete extensible_tiles->get(i);
  }
  delete extensible_tiles;

  SDL_FreeSurface(tileset_image);
  tile_number = 0;
}

TileData *Tileset::get_new_extensible_tile(int index, int repeat_x, int repeat_y) {
  ExtensibleTile *tile = new ExtensibleTile((ExtensibleTile*) get_tile(index), repeat_x, repeat_y);
  extensible_tiles->add(tile);
  return tile;
}
