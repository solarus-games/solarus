#include <SDL/SDL.h>
#include "game_resource.h"
#include "global.h"
#include "tilesets/house.h"
#include "maps/link_house.h"

GameResource::GameResource(void) {

}

void GameResource::create_resources(void) {
  // we cannot create the resources in the constructor
  // because some resources need other resources

  // tilesets
  tilesets[TILESET_HOUSE] = new TilesetHouse();

  // maps
  maps[MAP_LINK_HOUSE] = new MapLinkHouse();
}

GameResource::~GameResource(void) {
  delete tilesets[TILESET_HOUSE];
  delete maps[MAP_LINK_HOUSE];
}

zsdx_color_t GameResource::get_color(int r, int g, int b) {
  return SDL_MapRGB(zsdx_global.screen->format, r, g, b);
}

Tileset *GameResource::get_tileset(int tileset_id) {
  return tilesets[tileset_id];
}

Map *GameResource::get_map(int map_id) {
  return maps[map_id];
}
