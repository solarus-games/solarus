/*
 * Map generated automatically by the map editor.
 */

#include <SDL/SDL.h>
#include "Map.h"
#include "GameResource.h"
#include "Tile.h"
#include "Color.h"
#include "ZSDX.h"
#include "tilesets/TilesetHouse.h"
#include "maps/MapLinkHouseSecretRoom.h"

/**
 * Constructor.
 */
MapLinkHouseSecretRoom::MapLinkHouseSecretRoom(void):
  Map(320, 240,
  get_color(88, 72, 72),
  ZSDX::game_resource->get_tileset(TILESET_HOUSE),
  ZSDX::game_resource->get_music(MUSIC_VILLAGE)) {
}

/**
 * Loads the map.
 */
void MapLinkHouseSecretRoom::load(void) {
  Tile *tile;
  SDL_Rect position_in_map;

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  // link start position
  link_start_x = 80;
  link_start_y = 80;

#include "../src/maps/MapLinkHouseSecretRoomEntities.inc"
}
