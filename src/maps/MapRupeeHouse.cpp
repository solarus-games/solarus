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
#include "maps/MapRupeeHouse.h"

/**
 * Constructor.
 */
MapRupeeHouse::MapRupeeHouse(void):
  Map(464, 320,
  get_color(88, 72, 72),
  ZSDX::game_resource->get_tileset(TILESET_HOUSE),
  ZSDX::game_resource->get_music(MUSIC_MINI_GAME)) {
}

/**
 * Loads the map.
 */
void MapRupeeHouse::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  // link start position
  link_start_x = 80;
  link_start_y = 80;

#include "../src/maps/MapRupeeHouseEntities.inc"
}
