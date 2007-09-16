/*
 * Map generated automatically by the map editor.
 */

#include <SDL/SDL.h>
#include "Map.h"
#include "GameResource.h"
#include "Tile.h"
#include "ZSDX.h"
#include "tilesets/TilesetHouse.h"
#include "maps/MapLinkHouseSecretRoom.h"

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

  INITIAL_STATE(MUSIC_DEFAULT, 32, 128, 0);

  EXIT(LAYER_LOW, 0, 128, 8, 16, MAP_RUPEEHOUSE, 0);

#include "../src/maps/MapLinkHouseSecretRoomEntities.inc"
}
