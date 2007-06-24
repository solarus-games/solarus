#include <SDL/SDL.h>
#include "map.h"
#include "game_resource.h"
#include "maps/link_house.h"
#include "tilesets/TilesetHouse.h"
#include "tile.h"
#include "color.h"
#include "zsdx.h"

MapLinkHouse::MapLinkHouse(void):
  Map(320, 240,
      get_color(88, 72, 72),
      ZSDX::game_resource->get_tileset(TILESET_HOUSE),
      ZSDX::game_resource->get_music(MUSIC_VILLAGE)) {

}

void MapLinkHouse::load(void) {
  TileImage *tile_image;
  SDL_Rect position_in_map;

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  tile_image = tileset->get_tile(2);
  position_in_map.x = 32;
  position_in_map.y = 48;
  add_new_tile(tile_image, position_in_map, 16, 10);

  tile_image = tileset->get_tile(20);
  position_in_map.x = 0;
  position_in_map.y = 16;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 208;
  position_in_map.y = 16;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(21);
  position_in_map.x = 200;
  position_in_map.y = 16;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 312;
  position_in_map.y = 16;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(22);
  position_in_map.x = 0;
  position_in_map.y = 216;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 208;
  position_in_map.y = 216;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(23);
  position_in_map.x = 200;
  position_in_map.y = 216;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 312;
  position_in_map.y = 216;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(29);
  position_in_map.x = 8;
  position_in_map.y = 16;
  add_new_tile(tile_image, position_in_map, 24, 1);

  tile_image = tileset->get_tile(29);
  position_in_map.x = 216;
  position_in_map.y = 16;
  add_new_tile(tile_image, position_in_map, 12, 1);

  tile_image = tileset->get_tile(31);
  position_in_map.x = 8;
  position_in_map.y = 216;
  add_new_tile(tile_image, position_in_map, 24, 1);

  tile_image = tileset->get_tile(31);
  position_in_map.x = 216;
  position_in_map.y = 216;
  add_new_tile(tile_image, position_in_map, 12, 1);

  tile_image = tileset->get_tile(30);
  position_in_map.x = 0;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map, 1, 24);

  position_in_map.x = 208;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map, 1, 24);

  tile_image = tileset->get_tile(28);
  position_in_map.x = 200;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map, 1, 24);

  position_in_map.x = 312;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map, 1, 24);

  tile_image = tileset->get_tile(32);
  position_in_map.x = 8;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 216;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(33);
  position_in_map.x = 184;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 296;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(34);
  position_in_map.x = 8;
  position_in_map.y = 200;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 216;
  position_in_map.y = 200;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(35);
  position_in_map.x = 184;
  position_in_map.y = 200;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 296;
  position_in_map.y = 200;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(41);
  position_in_map.x = 24;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map, 10, 1);

  tile_image = tileset->get_tile(41);
  position_in_map.x = 232;
  position_in_map.y = 24;
  add_new_tile(tile_image, position_in_map, 4, 1);

  tile_image = tileset->get_tile(43);
  position_in_map.x = 24;
  position_in_map.y = 200;
  add_new_tile(tile_image, position_in_map, 10, 1);

  tile_image = tileset->get_tile(43);
  position_in_map.x = 232;
  position_in_map.y = 200;
  add_new_tile(tile_image, position_in_map, 4, 1);

  tile_image = tileset->get_tile(42);
  position_in_map.x = 8;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map, 1, 10);

  position_in_map.x = 216;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map, 1, 10);

  tile_image = tileset->get_tile(40);
  position_in_map.x = 184;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map, 1, 10);

  position_in_map.x = 296;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map, 1, 10);

  tile_image = tileset->get_tile(4);
  position_in_map.x = 24;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 232;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(5);
  position_in_map.x = 176;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 288;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(6);
  position_in_map.x = 24;
  position_in_map.y = 192;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 232;
  position_in_map.y = 192;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(7);
  position_in_map.x = 176;
  position_in_map.y = 192;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 288;
  position_in_map.y = 192;
  add_new_tile(tile_image, position_in_map);

  tile_image = tileset->get_tile(13);
  position_in_map.x = 32;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map, 18, 1);

  tile_image = tileset->get_tile(13);
  position_in_map.x = 240;
  position_in_map.y = 40;
  add_new_tile(tile_image, position_in_map, 6, 1);

  tile_image = tileset->get_tile(15);
  position_in_map.x = 32;
  position_in_map.y = 192;
  add_new_tile(tile_image, position_in_map, 18, 1);

  tile_image = tileset->get_tile(15);
  position_in_map.x = 240;
  position_in_map.y = 192;
  add_new_tile(tile_image, position_in_map, 6, 1);

  tile_image = tileset->get_tile(14);
  position_in_map.x = 24;
  position_in_map.y = 48;
  add_new_tile(tile_image, position_in_map, 1, 18);

  position_in_map.x = 232;
  position_in_map.y = 48;
  add_new_tile(tile_image, position_in_map, 1, 18);

  tile_image = tileset->get_tile(12);
  position_in_map.x = 176;
  position_in_map.y = 48;
  add_new_tile(tile_image, position_in_map, 1, 18);

  position_in_map.x = 288;
  position_in_map.y = 48;
  add_new_tile(tile_image, position_in_map, 1, 18);

  tile_image = tileset->get_tile(18);
  position_in_map.x = 0;
  position_in_map.y = 48;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 0;
  position_in_map.y = 176;
  add_new_tile(tile_image, position_in_map);
  
  tile_image = tileset->get_tile(16);
  position_in_map.x = 304;
  position_in_map.y = 48;
  add_new_tile(tile_image, position_in_map);

  position_in_map.x = 304;
  position_in_map.y = 176;
  add_new_tile(tile_image, position_in_map);
  
  /* test of animated tile:
  tile_image = tileset->get_tile(72);
  position_in_map.x = 48;
  position_in_map.y = 64;
  add_new_tile(tile_image, position_in_map);
  */

  // link start position
  link_start_x = 80;
  link_start_y = 80;
}
