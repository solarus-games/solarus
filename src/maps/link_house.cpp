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
  TileData *tile_data;

  if (!tileset->is_loaded()) {
    tileset->load();
  }

  tile_data = new ExtensibleTile((ExtensibleTile*) tileset->get_tile(24), 16, 9);
  {
    SDL_Rect where_in_map = {32, 48};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(0);
  {
    SDL_Rect where_in_map = {0, 16};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {208, 16};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(1);
  {
    SDL_Rect where_in_map = {184, 16};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {296, 16};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(2);
  {
    SDL_Rect where_in_map = {0, 200};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {208, 200};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(3);
  {
    SDL_Rect where_in_map = {184, 200};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {296, 200};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = new ExtensibleTile((ExtensibleTile*) tileset->get_tile(4), 10, 1);
  {
    SDL_Rect where_in_map = {24, 16};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = new ExtensibleTile((ExtensibleTile*) tileset->get_tile(4), 4, 1);
  {
    SDL_Rect where_in_map = {232, 16};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = new ExtensibleTile((ExtensibleTile*) tileset->get_tile(5), 10, 1);
  {
    SDL_Rect where_in_map = {24, 200};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = new ExtensibleTile((ExtensibleTile*) tileset->get_tile(5), 4, 1);
  {
    SDL_Rect where_in_map = {232, 200};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = new ExtensibleTile((ExtensibleTile*) tileset->get_tile(6), 1, 10);
  {
    SDL_Rect where_in_map = {0, 40};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {208, 40};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = new ExtensibleTile((ExtensibleTile*) tileset->get_tile(7), 1, 10);
  {
    SDL_Rect where_in_map = {184, 40};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {296, 40};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(16);
  {
    SDL_Rect where_in_map = {24, 40};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {232, 40};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(17);
  {
    SDL_Rect where_in_map = {176, 40};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {288, 40};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(18);
  {
    SDL_Rect where_in_map = {24, 192};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {232, 192};
    add_object(new Tile(tile_data, where_in_map));
  }

  tile_data = tileset->get_tile(19);
  {
    SDL_Rect where_in_map = {176, 192};
    add_object(new Tile(tile_data, where_in_map));
  }
  {
    SDL_Rect where_in_map = {288, 192};
    add_object(new Tile(tile_data, where_in_map));
  }

}
