#ifndef ZSDX_MAP_H
#define ZSDX_MAP_H

#include "color.h"
#include "map_entity.h"
#include "dynamic_array.h"
#include "tileset.h"
#include "music.h"

/* Obstacle types for the tiles
 */
enum tile_obstacle_t {
  NO_OBSTACLE,         // the tile is not an obstacle
  OBSTACLE,            // the tile is entirely an obstacle
  OBSTACLE_UP_RIGHT,   // the upper-right half of the tile is an obstacle
  OBSTACLE_UP_LEFT,    // the upper-left half of the tile is an obstacle
  OBSTACLE_DOWN_LEFT,  // the lower-left half of the tile is an obstacle
  OBSTACLE_DOWN_RIGHT, // the lower-right half of the tile is an obstacle
};

/* Abstract class for the maps
 * A map is where a game sequence takes place.
 */
class Map {

  // fields
 protected:
  /* Map width in pixels
   */
  const int width;

  /* Map height in pixels
   */
  const int height;
  
  /* Map width in 8*8 squares
   * width8 = width / 8
   */
  const int width8;

  /* Map height in 8*8 squares
   * height8 = height / 8
   */
  const int height8;

  /* Background color of the map
   */
  const zsdx_color_t background_color;

  /* Tileset of the map
   * Every tile of this map is extracted from this tileset
   */
  Tileset *tileset;

  /**
   * Background music of the map.
   */
  Music *background_music;

  /* X start position of Link
   * This will be changed soon because most of the map can have several
   * start positions for Link
   */
  int link_start_x;

  /* Y start position of Link
   */
  int link_start_y;

 private:
  /* All entities of the map (except Link)
   */
  DynamicArray<MapEntity*> *entities;

  /* Number of elements in the array obstacle_tiles
   * obstacle_tiles_size = width8 * height8
   */
  const int obstacle_tiles_size;

  /* Array of tile_obstacle_t representing which tiles
   * are obstacles and how
   */
  tile_obstacle_t *obstacle_tiles;

  // methods
 private:
  /* Display the map with all its entities except Link on the screen
   */
  void display(SDL_Surface *surface);

 protected:
  /* Add an entity into the map
   */
  inline void add_entity(MapEntity *entity) { entities->add(entity); }

  /* Add a tile into the map
   * This function should be called only when loading the map.
   * The tiles or a map are not supposed to change during the game.
   */
  void add_new_tile(TileData *tile_data, SDL_Rect &where_in_map);

 public:
  /* Constructor
   */
  Map(int width, int height, zsdx_color_t background_color,
      Tileset *tileset, Music *background_music);

  /* Destructor
   */
  virtual ~Map();

  /* Return the tileset of this map
   */
  inline Tileset *get_tileset(void) { return tileset; }

  /* Return the SDL Surface on which the map is displayed
   */
  SDL_Surface *get_surface(void);

  /* Load the map
   * This function is abstract so that each subclass can define its own map
   * by adding the appropriate entities.
   */
  virtual void load(void) = 0;

  /* Unload the map
   * Destroy all entities in the map to free some memory. This function
   * should be called when the player exists the map.
   */
  void unload(void);

  /* Launches the map.
   * Link is placed on the map and the player takes the control.
   */
  void start(void);

  /**
   * Exits the map.
   * This function is called when Link leaves the map.
   */
  void exit(void);

  /**
   * Test if the given point collides with a map tile.
   * The coordinates are in pixels.
   */
  bool collides_with(int x, int y);

  /**
   * Tests if the given rectangle collides with the map tiles
   * when moving to a specific direction. The direction permits
   * to make the test faster.
   * direction_mask should be 0, 45, 90, 135, 180, 225, 270 or 315
   * collision_box.w and collision_box.h should be multiples of 8
   * TODO killer feature: return something more precise than bool
   * too handle oblic walls!!!
   */
  bool collides_with(SDL_Rect &collision_box);
};

#endif
