/*
 * Map generated automatically by the map editor.
 */

#include "maps/MapLinkHouse.h"
#include "tilesets/TilesetHouse.h"

/**
 * Constructor.
 */
MapLinkHouse::MapLinkHouse(void):
  Map(384, 240,
    TILESET_HOUSE,
    MUSIC_VILLAGE) {

}

/**
 * Loads the map.
 */
void MapLinkHouse::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  ENTRANCE(MUSIC_DEFAULT, LAYER_LOW, 128, 216, 1);
  ENTRANCE(MUSIC_DEFAULT, LAYER_LOW, 352, 128, 2);

  EXIT(LAYER_LOW, 120, 240, 16, 16, MAP_LINKHOUSESECRETROOM, 0);

#include "../src/maps/MapLinkHouseEntities.inc"
}
