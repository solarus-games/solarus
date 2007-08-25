/**
 * This module defines the class Map.
 */

#ifndef ZSDX_MAP_H
#define ZSDX_MAP_H

using namespace std;
#include <iostream>
#include <vector>
#include "Types.h"
#include "Color.h"
#include "MapEntity.h"
#include "Tileset.h"
#include "Music.h"
#include "TileOnMap.h"

/**
 * Abstract class for the maps
 * A map is where a game sequence takes place. It contains many information:
 * - the map dimensions
 * - the background color
 * - the tileset
 * - the tiles
 * - the obstacles
 * - the background music
 */
class Map {

  // fields
 protected:

  /**
   * Map width in pixels.
   */
  const int width;

  /**
   * Map height in pixels.
   */
  const int height;
  
  /**
   * Map width in 8*8 squares.
   * width8 = width / 8
   */
  const int width8;
  
  /**
   * Map height in 8*8 squares.
   * height8 = height / 8
   */
  const int height8;

  /**
   * Background color of the map.
   */
  const zsdx_color_t background_color;

  /**
   * Tileset of the map.
   * Every tile of this map is extracted from this tileset.
   */
  Tileset *tileset;

  /**
   * Background music of the map.
   */
  Music *background_music;

  /**
   * X start position of Link.
   * TODO: most of the maps can have several start positions for Link
   */
  int link_start_x;

  /**
   * Y start position of Link.
   */
  int link_start_y;

 private:
  
  /**
   * All tiles of the map (a vector for each layer).
   */
  vector<TileOnMap*> *tiles[LAYER_NB];
  
  /**
   * Number of elements in the array obstacle_tiles.
   * obstacle_tiles_size = width8 * height8
   */
  const int obstacle_tiles_size;
  
  /**
   * Array of Obstacle representing which tiles are obstacles and how.
   */
  Obstacle *obstacle_tiles[LAYER_NB];

  /**
   * Position of the screen in the map.
   */
  SDL_Rect screen_position;

  // methods
  
  /**
   * Updates the animation and the position of each sprite, including Link.
   */
  void update_sprites(void);
  
  /**
   * Displays the map with all its entities on the screen.
   * @param surface the map surface
   */
  void display(SDL_Surface *surface);
  
 protected:

  /**
   * Adds an entity onto the map.
   * @param entity the entity to add
   */
  //  inline void add_entity(MapEntity *entity) { entities->push_back(entity); }

  /**
   * Creates a tile on the map.
   * It is equivalent to add_new_tile(tile_image, position_in_map, 1, 1).
   * This function is called for each tile, when loading the map.
   * The tiles on a map are not supposed to change during the game.
   * @param tile_image image of the tile to create
   * @param position_in_map position of the tile on the map
   * @param layer layer of the tile
   */
  void add_new_tile(Tile *tile, SDL_Rect &position_in_map, Layer layer);

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
  void add_new_tile(Tile *tile, SDL_Rect &position_in_map, Layer layer, int repeat_x, int repeat_y);

 public:

  /**
   * Creates a new map.
   * @param width the map width in pixels
   * @param height the map height in pixels
   * @param background_color the background_color
   * @param tileset the map tileset
   * @param background_music the map music
   */
  Map(int width, int height, zsdx_color_t background_color,
      Tileset *tileset, Music *background_music);

  /**
   * Destructor.
   */
  virtual ~Map();

  /**
   * Returns the tileset of this map.
   * @return the tileset
   */
  inline Tileset *get_tileset(void) { return tileset; }

  /**
   * Returns the SDL surface where the map is displayed (normally the screen).
   * @return the surface where the map is displayed
   */
  SDL_Surface *get_surface(void);
  
  /**
   * Returns the position of the screen in the map.
   * @return the position of the screen in the map
   */
  inline SDL_Rect *get_screen_position(void) { return &screen_position; }

  /**
   * Loads the map.
   * Creates the tiles and the other entities of the map.
   * This function is abstract so that each subclass can define its own map
   * by adding the appropriate entities.
   */
  virtual void load(void) = 0;

  /**
   * Unloads the map.
   * Destroys all entities in the map to free some memory. This function
   * can be called when the player exists the map. If the player is likely to
   * come back on the map, you can keep the map loaded.
   */
  void unload(void);

  /**
   * Launches the map. The map must be loaded.
   * Link is placed on the map, the player takes the control.
   * The SDL main loop is started.
   */
  void start(void);

  /**
   * Exits the map.
   * This function is called when Link leaves the map.
   */
  void exit(void);

  /**
   * Tests whether a point collides with a map tile.
 * @param layer layer of the point
   * @param x x of the point in pixels
   * @param y y of the point in pixels
   * @return the obstacle property of this point
   */
  Obstacle pixel_collision(int layer, int x, int y);

  /**
   * Tests whether a rectangle collides with the map tiles.
   * @param layer layer of the rectangle in the map
   * @param collision_box the rectangle to check
   * (its dimensions should be multiples of 8)
   * @return true if the rectangle is overlapping an obstacle, false otherwise
   */
  bool simple_collision(int layer, SDL_Rect &collision_box);

};

#endif
