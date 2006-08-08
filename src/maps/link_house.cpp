#include <SDL/SDL.h>
#include "map.h"
#include "global.h"
#include "game_resource.h"
#include "maps/link_house.h"
#include "tilesets/house.h"
#include "tile.h"
#include "color.h"

MapLinkHouse::MapLinkHouse(void):
  Map(320, 240, COLOR_BLACK, game_resource->get_tileset(TILESET_HOUSE)) {

}

void MapLinkHouse::load(void) {
  if (!tileset->is_loaded()) {
    tileset->load();
  }

  {
    TileData *tile_data = tileset->get_tile(0);
    SDL_Rect where_in_map = {0, 16};
    add_object(new Tile(tile_data, where_in_map));
  }
}
