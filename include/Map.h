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
#include "Tileset.h"
#include "Music.h"
#include "TileOnMap.h"
#include "MapExit.h"
#include "MapInitialState.h"

/**
 * A macro used by the generated code to put a tile on the map.
 */
#define TILE(tile_id, layer, x, y, repeat_x, repeat_y) add_new_tile(tile_id, layer, x, y, repeat_x, repeat_y)

/**
 * A macro used to define an initial state on the map.
 */
#define INITIAL_STATE(music_id, link_x, link_y, link_direction) add_initial_state(music_id, link_x, link_y, link_direction)

/**
 * A macro used to define an exit on the map.
 * When Link walks on the exit, he leaves the map and enters another one.
 */
#define EXIT(layer, x, y, w, h, map_id, initial_state) add_exit(layer, x, y, w, h, map_id, initial_state)

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

 private:
  
  /**
   * All tiles of the map (a vector for each layer).
   */
  vector<TileOnMap*> *tiles[LAYER_NB];
  
  /**
   * Array of Obstacle representing which tiles are obstacles and how.
   */
  Obstacle *obstacle_tiles[LAYER_NB];

  /**
   * Position of the screen in the map.
   */
  SDL_Rect screen_position;

  /**
   * Vector of all possible initial states of the map.
   */
  vector<MapInitialState*> *initial_states;

  /**
   * Initial state of the map.
   */
  unsigned int initial_state_index;

  /**
   * Vector of all entity detectors of the map.
   */
  vector<EntityDetector*> *entity_detectors;

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

 private:

  /**
   * Number of elements in the array obstacle_tiles.
   * obstacle_tiles_size = width8 * height8
   */
  const int obstacle_tiles_size;
  
 protected:

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
   * ID of the default music of the map (can be a valid music, MUSIC_NONE or MUSIC_NO_CHANGE).
   */
  const MusicID default_music_id;

  /**
   * Creates a tile on the map.
   * This function is called for each tile when loading the map.
   * The tiles on a map are not supposed to change during the game.
   * @param tile_id id of the tile in the tileset
   * @param layer layer of the tile to create
   * @param position_in_map x position of the tile on the map
   * @param position_in_map y position of the tile on the map
   * @param repeat_x how many times the tile pattern is repeated on x
   * @param repeat_x how many times the tile pattern is repeated on y
   */
  void add_new_tile(int tile_id, Layer layer, int x, int y, int repeat_x, int repeat_y);

  /**
   * Creates an initial state on the map.
   * @param music_id id of the music to play in this state
   * (can be a real music, MUSIC_NONE, MUSIC_NO_CHANGE or MUSIC_DEFAULT)
   * @param link_x x initial position of link in this state
   * (set -1 to indicate that the x coordinate is kept the same from the previous map)
   * @param link_y y initial position of link in this state
   * (set -1 to indicate that the y coordinate is kept the same from the previous map)
   * @param link_direction initial direction of link in this state (0 to 3)
   */
  void add_initial_state(MusicID music_id, int link_x, int link_y, int link_direction);

  /**
   * Creates an exit on the map.
   * When Link walks on the exit, he leaves the map and enters another one.
   * @param layer layer of the exit to create
   * @param x x position of the exit rectangle
   * @param y y position of the exit rectangle
   * @param w width of the exit rectangle
   * @param h height of the exit rectangle
   * @param map_id id of the next map
   * @param initial_state_index initial state of the next map
   */
  void add_exit(Layer layer, int x, int y, int w, int h, MapID map_id, int initial_state_index);

 public:

  /**
   * Creates a new map.
   * @param width the map width in pixels
   * @param height the map height in pixels
   * @param background_color the background_color
   * @param tileset_id the map tileset
   * @param default_music_id the map music
   */
  Map(int width, int height, zsdx_color_t background_color,
      TilesetID tileset_id, MusicID default_music_id);

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
   * Starts the map. The map must be loaded.
   * Link is placed on the map and the background music starts.
   */
  void start(void);

  /**
   * Exits the map.
   * This function is called when Link leaves the map.
   */
  void leave(void);

  /**
   * Sets the initial state of the map when it is loaded.
   * @param initial_state index of the initial state you want to load
   */
  void set_initial_state(unsigned int initial_state_index);

  /**
   * Updates the animation and the position of each sprite, including Link.
   */
  void update_sprites(void);
  
  /**
   * Displays the map with all its entities on the screen.
   */
  void display();
  
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
  bool collision_with_tiles(int layer, SDL_Rect &collision_box);

  /**
   * This function is called by an entity sensible to the obstacles
   * when this entity has just moved on the map.
   * We check whether or not the entity overlaps an entity detector.
   * @param entity the entity that has just moved
   */
  void entity_just_moved(MovingWithCollision *entity);

  // Events

  /**
   * This function is called by the game engine when an entity overlaps a detector.
   * Redefine this function to create a behavior for your detectors.
   * @param detector the detector
   * @param entity the entity
   */
  virtual void event_entity_on_detector(EntityDetector *dectector, MapEntity *entity);

};

#endif
