#include <SDL/SDL.h>
#include "map.h"
#include "tile.h"
#include "zsdx.h"
using namespace std;
#include <iostream>

const int FRAMES_PER_SECOND = 50;
const int FRAME_DELAY = 1000 / FRAMES_PER_SECOND; // delay between two frames in millisecond

Map::Map(int width, int height, zsdx_color_t background_color,
	 Tileset *tileset, Music *background_music):
  width(width), height(height), width8(width / 8), height8(height / 8),
  background_color(background_color),
  tileset(tileset),
  background_music(background_music),
  entities(new DynamicArray<MapEntity*>(256)),
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
  delete entities;
  delete[] obstacle_tiles;
}

void Map::add_new_tile(TileData *tile_data, SDL_Rect &where_in_map) {
  // add the tile to the map objects
  add_entity(new Tile(tile_data, where_in_map));

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
      *obstacle = tile_data->get_obstacle();
    }
  }
}

void Map::unload(void) {
  for (int i = 0; i < entities->get_size(); i++) {
    delete entities->get(i);
  }
  entities->clear();
}

SDL_Surface *Map::get_surface(void) {
  return zsdx.get_screen();
}

void Map::display(SDL_Surface *surface) {
  // for now we don't care about the scrolling

  // background color
  SDL_FillRect(surface, NULL, background_color);

  // map objects
  for (int i = 0; i < entities->get_size(); i++) {
    entities->get(i)->display_on_map(this);
  }

  // link
  game_resource->get_link()->display_on_map(this);

  SDL_Flip(surface);
}

void Map::start(void) {
  SDL_Event event;
  Uint32 ticks, last_frame_date = 0;
  bool quit = false;

  // start the background music
  if (background_music != NULL) {
    background_music->play();
  }

  Link *link = game_resource->get_link();
  link->set_map(this);
  link->set_position(link_start_x, link_start_y);

  //  SDL_EnableKeyRepeat(5, 10);

  while (!quit) {
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
	
      case SDL_QUIT:
	quit = true;
	break;
	
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {

	  // pour martin
// 	case SDLK_SPACE:
// 	  background_music->set_paused(!background_music->is_paused());
// 	  break;

	case SDLK_ESCAPE:
	  quit = true;
	  break;

	case SDLK_RIGHT:
	  link->start_right();
	  break;

	case SDLK_UP:
	  link->start_up();
	  break;

	case SDLK_LEFT:
	  link->start_left();
	  break;

	case SDLK_DOWN:
	  link->start_down();
	  break;

	default:
	  break;
	}
	break;
	
      case SDL_KEYUP:
	switch (event.key.keysym.sym) {
	case SDLK_RIGHT:
	  link->stop_right();
	  break;
	case SDLK_UP:
	  link->stop_up();
	  break;
	case SDLK_LEFT:
	  link->stop_left();
	  break;
	case SDLK_DOWN:
	  link->stop_down();
	  break;
	default:
	  break;
	}
	break;

	// check the obstacles
//     case SDL_MOUSEBUTTONDOWN:
// 	if (event.button.button == SDL_BUTTON_LEFT) {
// 	  int x = event.button.x;
// 	  int y = event.button.y;
// 	  tile_obstacle_t obstacle = obstacle_tiles[width8*(y/8) + (x/8)];
// 	  cout << "obstacle: " << obstacle << "\n";
// 	}
// 	break;
      }
    }

    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + FRAME_DELAY) {
      last_frame_date = ticks;
      display(zsdx.get_screen());
    }
  }
}

void Map::exit(void) {
  if (background_music != NULL) {
    background_music->stop();
  }
}

/**
 * Returns true if the given point collides with an obstacle.
 * The coordinates are in pixels.
 */
tile_obstacle_t Map::pixel_collision(int x, int y) {
  tile_obstacle_t obstacle_type;
  bool on_obstacle;
  int x_in_tile, y_in_tile;

  obstacle_type = obstacle_tiles[(y / 8) * width8 + (x / 8)];

  switch (obstacle_type) {

  case NO_OBSTACLE:
    on_obstacle = false;
    break;

  case OBSTACLE:
    on_obstacle = true;

    break;
  case OBSTACLE_UP_RIGHT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > x_in_tile;
    break;

  case OBSTACLE_UP_LEFT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > 1 - x_in_tile;
    break;
    
  case OBSTACLE_DOWN_LEFT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < x_in_tile;
    break;
    
  case OBSTACLE_DOWN_RIGHT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < 1 - x_in_tile;
    break;
  }

  return on_obstacle ? obstacle_type : NO_OBSTACLE;
}

bool Map::simple_collision(SDL_Rect &collision_box) {
  int x1, x2, y1, y2;
  bool collision;
  

  // uncomment the following line to show the collision box
  //  SDL_FillRect(zsdx.get_screen(), &collision_box, background_color);
  
  // we check the 4 corners of each 8*8 square in the collision box
  for (y1 = collision_box.y; y1 < collision_box.y + collision_box.h && !collision; y1 += 8) {
    y2 = y1 + 7;
    for (x1 = collision_box.x; x1 < collision_box.x + collision_box.w && !collision; x1 += 8) {
      x2 = x1 + 7;
      collision = pixel_collision(x1, y1) || pixel_collision(x2, y1)
	|| pixel_collision(x1, y2) || pixel_collision(x2, y2);
    }
  }
  
  return collision;   
}
