/**
 * This module defines the class Map.
 */

using namespace std;
#include <iostream>
#include <SDL/SDL.h>
#include "Map.h"
#include "TileOnMap.h"
#include "ZSDX.h"

/**
 * Creates a new map.
 * @param width the map width in pixels
 * @param height the map height in pixels
 * @param background_color the background_color
 * @param tileset the map tileset
 * @param background_music the map music
 */
Map::Map(int width, int height, zsdx_color_t background_color,
	 Tileset *tileset, Music *background_music):

  width(width), height(height), width8(width / 8), height8(height / 8),
  background_color(background_color), tileset(tileset),
  background_music(background_music),
  entities(new vector<MapEntity*>()),
  obstacle_tiles_size(width8 * height8),
  obstacle_tiles(new Obstacle[obstacle_tiles_size]) {

  // initialize obstacle tile
  for (int i = 0; i < obstacle_tiles_size; i++) {
    obstacle_tiles[i] = OBSTACLE_NONE;
  }
}

/**
 * Destructor.
 */
Map::~Map() {
  unload();
  delete entities;
  delete[] obstacle_tiles;
}

/**
 * Creates a tile on the map.
 * It is equivalent to add_new_tile(tile, position_in_map, 1, 1).
 * This function is called for each tile, when loading the map.
 * The tiles on a map are not supposed to change during the game.
 * @param tile_image image of the tile to create
 * @param position_in_map position of the tile on the map
 * @param layer layer of the tile
 */
void Map::add_new_tile(Tile *tile, SDL_Rect &position_in_map, Layer layer) {
  add_new_tile(tile, position_in_map, layer, 1, 1);
}

/**
 * Creates a tile on the map, repeating its pattern.
 * This function is called for each tile, when loading the map.
 * The tiles on a map are not supposed to change during the game.
 * @param tile_image image of the tile to create
 * @param position_in_map position of the tile on the map
 * @param layer layer of the tile
 * @param repeat_x how many times the pattern is repeated on x
 * @param repeat_x how many times the pattern is repeated on y
 */
void Map::add_new_tile(Tile *tile, SDL_Rect &position_in_map, Layer layer, int repeat_x, int repeat_y) {
  // add the tile to the map objects
  add_entity(new TileOnMap(tile, position_in_map, layer, repeat_x, repeat_y));

  // update the collision list
  int tile_x8 = position_in_map.x / 8;
  int tile_y8 = position_in_map.y / 8;
  int tile_width8 = (tile->get_width() / 8) * repeat_x;
  int tile_height8 = (tile->get_height() / 8) * repeat_y;

  // we traverse each 8*8 square in the tile
  Obstacle *obstacle;
  int index;
  for (int j = 0; j < tile_height8; j++) {
    index = (tile_y8 + j) * width8 + tile_x8;
    for (int i = 0; i < tile_width8; i++) {
      obstacle = &obstacle_tiles[index++];
      *obstacle = tile->get_obstacle();
    }
  }
}

/**
 * Unloads the map.
 * Destroys all entities in the map to free some memory. This function
 * can be called when the player exists the map. If the player is likely to
 * come back on the map, you can keep the map loaded.
 */
void Map::unload(void) {
  for (unsigned int i = 0; i < entities->size(); i++) {
    delete entities->at(i);
  }
  entities->clear();
}

/**
 * Returns the SDL surface where the map is displayed (normally the screen).
 * @return the surface where the map is displayed
 */
SDL_Surface *Map::get_surface(void) {
  return ZSDX::get_screen();
}

/**
 * Updates the animation and the position of each sprite, including Link.
 */
void Map::update_sprites(void) {
  ZSDX::game_resource->get_link()->update_position();
}

/**
 * Displays the map with all its entities except Link on the screen.
 * @param surface the map surface
 */
void Map::display(SDL_Surface *surface) {
  // TODO: handle the scrolling

  // background color
  SDL_FillRect(surface, NULL, background_color);

  // map entities
  for (unsigned int i = 0; i < entities->size(); i++) {
    entities->at(i)->display_on_map(this);
  }

  // link
  ZSDX::game_resource->get_link()->display_on_map(this);

  SDL_Flip(surface);
}

/**
 * Launches the map. The map must be loaded.
 * Link is placed on the map, the player takes the control.
 * The SDL main loop is started.
 */
void Map::start(void) {
  SDL_Event event;
  Uint32 ticks, last_frame_date = 0;
  bool quit = false;

  // start the background music
  if (background_music != NULL) {
    background_music->play();
  }

  // put Link
  Link *link = ZSDX::game_resource->get_link();
  link->set_map(this);
  link->set_position(link_start_x, link_start_y);


  //  SDL_EnableKeyRepeat(5, 10);

  // SDL main loop
  while (!quit) {
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
	
	// quit if the user closes the window
      case SDL_QUIT:
	quit = true;
	break;
	
	// a key is pressed
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {

	  // escape: quit
	case SDLK_ESCAPE:
	  quit = true;
	  break;
	  
	  // F5: full screen / windowed mode
	case SDLK_F5:
	  ZSDX::switch_fullscreen();
	  break;

	  // move Link
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
	  
	  // space: pause the music
	case SDLK_SPACE:
	  background_music->set_paused(!background_music->is_paused());
	  break;

	default:
	  break;
	}
	break;
	
	// stop Link's movement
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
// 	  Obstacle obstacle = obstacle_tiles[width8*(y/8) + (x/8)];
// 	  cout << "obstacle: " << obstacle << "\n";
// 	}
// 	break;
      }
    }
    
    // update the sprites animations and positions
    update_sprites();

    // display everything each time frame
    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + FRAME_INTERVAL) {
      last_frame_date = ticks;
      display(get_surface());
    }
  }
}

/**
 * Exits the map.
 * This function is called when Link leaves the map.
 */
void Map::exit(void) {
  if (background_music != NULL) {
    background_music->stop();
  }
}

/**
 * Tests whether a point collides with a map tile.
 * @param x x of the point in pixels
 * @param y y of the point in pixels
 * @return the obstacle property of this point
 */
Obstacle Map::pixel_collision(int x, int y) {
  Obstacle obstacle_type;
  bool on_obstacle;
  int x_in_tile, y_in_tile;

  // get the obstacle property of this point's 8*8 square
  obstacle_type = obstacle_tiles[(y / 8) * width8 + (x / 8)];

  // test the obstacle property of this square
  switch (obstacle_type) {

  case OBSTACLE_NONE:
    // the square is not an obstacle
    on_obstacle = false;
    break;

  case OBSTACLE:
    // the square is entirely an obstacle
    on_obstacle = true;
    break;

  case OBSTACLE_TOP_RIGHT:
    // the upper right half of the square is an obstacle
    // sp we have to test the position of the point
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > x_in_tile;
    break;

  case OBSTACLE_TOP_LEFT:
    // same thing
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile > 1 - x_in_tile;
    break;
    
  case OBSTACLE_BOTTOM_LEFT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < x_in_tile;
    break;
    
  case OBSTACLE_BOTTOM_RIGHT:
    x_in_tile = x % 8;
    y_in_tile = y % 8;
    on_obstacle = y_in_tile < 1 - x_in_tile;
    break;
  }

  return on_obstacle ? obstacle_type : OBSTACLE_NONE;
}

/**
 * Tests whether a rectangle collides with the map tiles.
 * @param collision_box the rectangle to check
 * (its dimensions should be multiples of 8)
 * @return true if the rectangle is overlapping an obstacle, false otherwise
 */
bool Map::simple_collision(SDL_Rect &collision_box) {
  int x1, x2, y1, y2;
  bool collision;

  // debug: uncomment the following line to show the collision box
  // SDL_FillRect(zsdx::get_screen(), &collision_box, background_color);
  
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
