/*
 * Map generated automatically by the map editor.
 */

#include "maps/MapLinkHouseSecretRoom.h"
#include "tilesets/TilesetHouse.h"

/**
 * Constructor.
 */
MapLinkHouseSecretRoom::MapLinkHouseSecretRoom(void):
  Map(320, 240,
    TILESET_HOUSE,
    MUSIC_VILLAGE) {

}

/**
 * Loads the map.
 */
void MapLinkHouseSecretRoom::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  ENTRANCE(LAYER_LOW, 32, 128, 0);

  EXIT(LAYER_LOW, 0, 128, 8, 16, MAP_RUPEEHOUSE, 0);

#include "../src/maps/MapLinkHouseSecretRoomEntities.inc"
}
