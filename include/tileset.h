#ifndef ZSDX_TILESET_H
#define ZSDX_TILESET_H

#include <SDL/SDL.h>
#include "tile_data.h"
#include "dynamic_array.h"

class Tileset {
  
 private:
  DynamicArray<ExtensibleTile*> *extensible_tiles;

 protected:
  TileData *tiles[1024];
  int tile_number;
  SDL_Surface *tileset_image;
  
 public:
  Tileset(void);
  virtual ~Tileset(void);

  void load_tileset_image(const char *file);
  virtual void load(void) = 0;
  void unload(void);
  inline bool is_loaded(void) { return tile_number != 0; }

  inline TileData *get_tile(int index) { return tiles[index]; }
  TileData *get_new_extensible_tile(int index, int repeat_x, int repeat_y);
};

#endif
