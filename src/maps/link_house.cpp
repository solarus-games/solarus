#include <SDL/SDL.h>
#include "map.h"
#include "global.h"
#include "game_resource.h"
#include "maps/link_house.h"
#include "tilesets/house.h"
#include "tile.h"
#include "color.h"

MapLinkHouse::MapLinkHouse(void):
  Map(320, 240,
      game_resource->get_color(88, 72, 72),
      game_resource->get_tileset(TILESET_HOUSE)) {

}

void MapLinkHouse::load(void) {
  TileData *tile_data;
  SDL_Rect where_in_map;

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  tile_data = tileset->get_new_extensible_tile(2, 16, 10);
  where_in_map.x = 32;
  where_in_map.y = 48;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(20);
  where_in_map.x = 0;
  where_in_map.y = 16;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 208;
  where_in_map.y = 16;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(21);
  where_in_map.x = 200;
  where_in_map.y = 16;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 312;
  where_in_map.y = 16;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(22);
  where_in_map.x = 0;
  where_in_map.y = 216;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 208;
  where_in_map.y = 216;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(23);
  where_in_map.x = 200;
  where_in_map.y = 216;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 312;
  where_in_map.y = 216;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(29, 24, 1);
  where_in_map.x = 8;
  where_in_map.y = 16;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(29, 12, 1);
  where_in_map.x = 216;
  where_in_map.y = 16;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(31, 24, 1);
  where_in_map.x = 8;
  where_in_map.y = 216;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(31, 12, 1);
  where_in_map.x = 216;
  where_in_map.y = 216;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(30, 1, 24);
  where_in_map.x = 0;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 208;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(28, 1, 24);
  where_in_map.x = 200;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 312;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(32);
  where_in_map.x = 8;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 216;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(33);
  where_in_map.x = 184;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 296;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(34);
  where_in_map.x = 8;
  where_in_map.y = 200;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 216;
  where_in_map.y = 200;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(35);
  where_in_map.x = 184;
  where_in_map.y = 200;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 296;
  where_in_map.y = 200;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(41, 10, 1);
  where_in_map.x = 24;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(41, 4, 1);
  where_in_map.x = 232;
  where_in_map.y = 24;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(43, 10, 1);
  where_in_map.x = 24;
  where_in_map.y = 200;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(43, 4, 1);
  where_in_map.x = 232;
  where_in_map.y = 200;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(42, 1, 10);
  where_in_map.x = 8;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 216;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(40, 1, 10);
  where_in_map.x = 184;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 296;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(4);
  where_in_map.x = 24;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 232;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(5);
  where_in_map.x = 176;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 288;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(6);
  where_in_map.x = 24;
  where_in_map.y = 192;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 232;
  where_in_map.y = 192;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(7);
  where_in_map.x = 176;
  where_in_map.y = 192;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 288;
  where_in_map.y = 192;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(13, 18, 1);
  where_in_map.x = 32;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(13, 6, 1);
  where_in_map.x = 240;
  where_in_map.y = 40;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(15, 18, 1);
  where_in_map.x = 32;
  where_in_map.y = 192;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(15, 6, 1);
  where_in_map.x = 240;
  where_in_map.y = 192;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(14, 1, 18);
  where_in_map.x = 24;
  where_in_map.y = 48;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 232;
  where_in_map.y = 48;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_new_extensible_tile(12, 1, 18);
  where_in_map.x = 176;
  where_in_map.y = 48;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 288;
  where_in_map.y = 48;
  add_new_tile(tile_data, where_in_map);

  tile_data = tileset->get_tile(18);
  where_in_map.x = 0;
  where_in_map.y = 48;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 0;
  where_in_map.y = 176;
  add_new_tile(tile_data, where_in_map);
  
  tile_data = tileset->get_tile(16);
  where_in_map.x = 304;
  where_in_map.y = 48;
  add_new_tile(tile_data, where_in_map);

  where_in_map.x = 304;
  where_in_map.y = 176;
  add_new_tile(tile_data, where_in_map);
  
  // link start position
  link_start_x = 80;
  link_start_y = 80;
}
