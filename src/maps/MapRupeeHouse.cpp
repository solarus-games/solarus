/*
 * Map generated automatically by the map editor.
 */

#include "maps/MapRupeeHouse.h"
#include "tilesets/TilesetHouse.h"
#include "maps/MapLinkHouse.h"

/**
 * Loads the map.
 */
void MapRupeeHouse::load(void) {

#include "../src/maps/MapRupeeHouseEntities.inc"

  ENTRANCE(LAYER_LOW, 120, 296, 1);

  EXIT(LAYER_LOW, 120, 320, 16, 16, MAP_LINKHOUSE, MAP_LINKHOUSE_INITIAL_STATE_EAST);

}
