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

  INITIAL_STATE(MUSIC_DEFAULT, 16, 128, 0);

  EXIT(LAYER_LOW, -16, 128, 8, 16, MAP_RUPEEHOUSE, 0);

#include "../src/maps/MapLinkHouseSecretRoomEntities.inc"
}
