/*
 * Map generated automatically by the map editor.
 */

#include <SDL/SDL.h>
#include "Map.h"
#include "GameResource.h"
#include "Tile.h"
#include "ZSDX.h"
#include "tilesets/TilesetHouse.h"
#include "maps/MapRupeeHouse.h"
#include "maps/MapLinkHouse.h"

/**
 * Constructor.
 */
MapRupeeHouse::MapRupeeHouse(void):
  Map(464, 320,
    TILESET_HOUSE,
    MUSIC_MINI_GAME) {

}

/**
 * Loads the map.
 */
void MapRupeeHouse::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  ENTRANCE(MUSIC_DEFAULT, LAYER_LOW, 120, 296, 1);

  EXIT(LAYER_LOW, 120, 320, 16, 16, MAP_LINKHOUSE, MAP_LINKHOUSE_INITIAL_STATE_EAST);

#include "../src/maps/MapRupeeHouseEntities.inc"
}
