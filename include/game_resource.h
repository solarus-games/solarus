#ifndef ZSDX_GAME_RESOURCE_H
#define ZSDX_GAME_RESOURCE_H

#include "map.h"
#include "tileset.h"

// tilesets
static const int TILESET_HOUSE = 0;

// maps
static const int MAP_LINK_HOUSE = 0;

struct GameResource {
  Tileset *tilesets[1];
  Map *maps[1];
  // musics
  // sounds
  // items
};

extern GameResource game_resource;

void create_game_resource(void);
void destroy_game_resource(void);

#endif
