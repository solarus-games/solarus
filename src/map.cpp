#include <SDL/SDL.h>
#include "datatypes.h"
#include "game_resource.h"
#include "map.h"
#include "map_object.h"
#include "tile.h"
#include "sdl_user_events.h"
#include "global.h"
#include "dynamic_array.h"

Map::Map(int width, int height, zsdx_color_t background_color,
	 Tileset *tileset):
  width(width), height(height), width8(width / 8), height8(height / 8),
  background_color(background_color),
  tileset(tileset),
  objects(new DynamicArray<MapObject*>(256)),
  obstacle_tiles_size(width8 * height8),
  obstacle_tiles(new tile_obstacle_t[obstacle_tiles_size])
{
  // no obstacle tile
  for (int i = 0; i < obstacle_tiles_size; i++) {
    obstacle_tiles[i] = NO_OBSTACLE;
  }
}

Map::~Map() {
  unload();
  delete objects;
  delete[] obstacle_tiles;
}

void Map::add_new_tile(TileData *tile_data, SDL_Rect &where_in_map) {
  // add the tile to the map objects
  add_object(new Tile(tile_data, where_in_map));

  // update the collision list
  int tile_x8 = where_in_map.x / 8;
  int tile_y8 = where_in_map.y / 8;
  int tile_width8 = tile_data->get_width() / 8;
  int tile_height8 = tile_data->get_height() / 8;

  // we traverse each 8*8 square in the tile
  tile_obstacle_t *obstacle;
  int index;
  for (int j = 0; j < tile_height8; j++) {
    index = (tile_y8 + j) * width8 + tile_x8;
    for (int i = 0; i < tile_width8; i++) {
      obstacle = &obstacle_tiles[index++];
      if (*obstacle != NO_OBSTACLE) {
	*obstacle = tile_data->get_obstacle();
      }
    }
  }
}

void Map::unload(void) {
  for (int i = 0; i < objects->get_size(); i++) {
    delete objects->get(i);
  }
  objects->clear();
}

void Map::display(SDL_Surface *surface) {
  // for now we don't care about the scrolling

  // background color
  SDL_FillRect(surface, NULL, background_color);

  // map objects
  for (int i = 0; i < objects->get_size(); i++) {
    objects->get(i)->display_on_map(surface);
  }

  // link
  game_resource->get_link()->display(surface);

  SDL_Flip(surface);
}

void Map::launch(void) {
  SDL_Event event;
  bool quit = false;

  Link *link = game_resource->get_link();
  link->set_position(link_start_x, link_start_y);

  SDL_EnableKeyRepeat(5, 10);

  while (!quit) {
    SDL_WaitEvent(&event);
    switch (event.type) {
    
    case SDL_QUIT:
      quit = true;
      break;
    
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
	quit = true;
	break;
      case SDLK_RIGHT:
	link->move_right();
	break;
      case SDLK_UP:
	link->move_up();
	break;
      case SDLK_LEFT:
	link->move_left();
	break;
      case SDLK_DOWN:
	link->move_down();
	break;
      default:
	break;
      }
      break;
    
    case SDL_USEREVENT:
      switch (event.user.type) {
      case EVENT_ANIMATION_FRAME_FINISHED:
	// just redraw the screen
	break;
      }
      break;
    }
    display(zsdx_global.screen);
  }
}
