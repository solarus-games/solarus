#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "map.h"
#include "tileset.h"

// tilesets
static const int TILESET_HOUSE = 0;

// maps
static const int MAP_LINK_HOUSE = 0;

class GameResource {

 private:
  Tileset *tilesets[1];
  Map *maps[1];
  // musics
  // sounds
  // items

 public:
  GameResource(void);
  ~GameResource(void);

  void create_resources(void);

  zsdx_color_t get_color(int r, int g, int b);
  Tileset *get_tileset(int tileset_id);
  Map *get_map(int map_id);

};

extern GameResource *game_resource;

#endif
