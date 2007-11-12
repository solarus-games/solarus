/*
 * Map generated automatically by the map editor.
 */

#include "maps/MapLinkHouseSecretRoom.h"
#include "tilesets/TilesetHouse.h"

/**
 * Loads the map.
 */
void MapLinkHouseSecretRoom::load(void) {

#include "../src/maps/MapLinkHouseSecretRoomEntities.inc"

  ENTRANCE(LAYER_LOW, 32, 128, 0);

  EXIT(LAYER_LOW, 0, 128, 8, 16, MAP_RUPEEHOUSE, 0);
}
