#include "tileset.h"

Tileset::~Tileset(void) {
  for (int i = 0; i < tile_number; i++) {
    delete tiles[i];
  }
}

TileData Tileset::*get_tile(int index) {
  return tiles[index];
}
