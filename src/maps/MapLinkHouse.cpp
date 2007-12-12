/*
 * Map generated automatically by the map editor.
 */

#include "maps/MapLinkHouse.h"
#include "tilesets/TilesetHouse.h"

/**
 * Loads the map.
 */
void MapLinkHouse::load(void) {

#include "../src/maps/MapLinkHouseEntities.inc"

  ENTRANCE(LAYER_LOW, 128, 200, 1);
  ENTRANCE(LAYER_LOW, 352, 128, 2);

  EXIT(LAYER_LOW, 120, 240, 16, 16, MAP_LINKHOUSESECRETROOM, 0);
}
