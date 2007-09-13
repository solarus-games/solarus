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
  Map(384, 240,
      get_color(88, 72, 72),
      TILESET_HOUSE,
      MUSIC_VILLAGE) {
  // TODO: when the map size is changed in the map editor, the values above are not updated
}

/**
 * Loads the map.
 */
void MapLinkHouse::load(void) {

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  INITIAL_STATE(MUSIC_DEFAULT, 128, 216, 1);
  INITIAL_STATE(MUSIC_DEFAULT, 352, 128, 2);

  EXIT(LAYER_LOW, 120, 240, 16, 16, MAP_LINKHOUSESECRETROOM, 0);

#include "../src/maps/MapLinkHouseEntities.inc"
}
