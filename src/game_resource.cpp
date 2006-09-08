#include <SDL/SDL.h>
#include "game_resource.h"
#include "link.h"
#include "tilesets/house.h"
#include "maps/link_house.h"

void GameResource::create_resources(void) {
  // we cannot create the resources in the constructor
  // because some resources need other resources

  // link
  link = new Link();

  // tilesets
  tilesets[TILESET_HOUSE] = new TilesetHouse();

  // maps
  maps[MAP_LINK_HOUSE] = new MapLinkHouse();
}

GameResource::~GameResource(void) {
  delete link;
  delete tilesets[TILESET_HOUSE];
  delete maps[MAP_LINK_HOUSE];
}

Tileset *GameResource::get_tileset(int tileset_id) {
  return tilesets[tileset_id];
}

Map *GameResource::get_map(int map_id) {
  return maps[map_id];
}
