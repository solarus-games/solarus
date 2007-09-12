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
#include "maps/MapLinkHouse.h"

/**
 * Constructor.
 */
MapLinkHouse::MapLinkHouse(void):
  Map(320, 240,
      get_color(88, 72, 72),
      TILESET_HOUSE,
      MUSIC_VILLAGE),
  initial_state_south(112, 216, 1),
  initial_state_east(304, 128, 2) {

  default_initial_state = &initial_state_south;
}

/**
 * Loads the map.
 */
void MapLinkHouse::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

#include "../src/maps/MapLinkHouseEntities.inc"
}
