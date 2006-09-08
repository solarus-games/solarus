#ifndef ZSDX_MAP_H
#define ZSDX_MAP_H

#include "color.h"
#include "map_entity.h"
#include "dynamic_array.h"
#include "tileset.h"

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
      Tileset *tileset);

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

  /* Launches the map
   * Link is placed on the map and the player takes the control.
   */
  void launch(void);
};

#endif
