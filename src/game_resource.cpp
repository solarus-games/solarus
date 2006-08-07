#include "game_resource.h"
#include "tilesets/house.h"
#include "maps/link_house.h"

GameResource game_resource;

void create_game_resource(void) {
  // tilesets
  game_resource.tilesets[TILESET_HOUSE] = new TilesetHouse();

  // maps
  game_resource.maps[MAP_LINK_HOUSE] = new MapLinkHouse();
}

void destroy_game_resource(void) {
  delete game_resource.tilesets[TILESET_HOUSE];
  delete game_resource.maps[MAP_LINK_HOUSE];
}
