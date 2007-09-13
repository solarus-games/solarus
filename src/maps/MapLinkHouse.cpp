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
      MUSIC_VILLAGE) {

}

/**
 * Loads the map.
 */
void MapLinkHouse::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  INITIAL_STATE(MUSIC_DEFAULT, 112, 216, 1);
  INITIAL_STATE(MUSIC_DEFAULT, 304, 128, 2);

  EXIT(LAYER_LOW, 104, 240, 16, 16, MAP_LINKHOUSESECRETROOM, 0);

#include "../src/maps/MapLinkHouseEntities.inc"
}
