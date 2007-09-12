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
      TILESET_HOUSE,
      MUSIC_MINI_GAME),
  initial_state(120, 296, 1) {

  default_initial_state = &initial_state;
}

/**
 * Loads the map.
 */
void MapRupeeHouse::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

#include "../src/maps/MapRupeeHouseEntities.inc"
}
