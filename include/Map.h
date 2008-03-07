/**
 * This module defines the class Map.
 */

#ifndef ZSDX_MAP_H
#define ZSDX_MAP_H

#include <vector>
#include "Common.h"

/**
 * Abstract class for the maps.
 * A map is where a game sequence takes place. It contains many information:
 * - the map dimensions
 * - the tileset
 * - the tiles
 * - the obstacles
 * - the background music
 */
class Map {

 private:

  /**
   * Id of the map.
   */
  MapId id;
  
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
   * Surface where the map is displayed.
   * This surface is only the visible part of the map, so the
   * coordinates on this surface are relative to the screen,
   * not to the map.
   */
  SDL_Surface *visible_surface;

  /**
   * Vector of all possible entrances of the map.
   */
  vector<MapEntrance*> *entrances;

  /**
   * Current entrance of the map.
   */
  unsigned int entrance_index;

  /**
   * Vector of all entity detectors of the map.
   */
  vector<EntityDetector*> *entity_detectors;

  /**
   * Map width in pixels.
   */
  int width;

  /**
   * Map height in pixels.
   */
  int height;
  
  /**
   * Map width in 8*8 squares.
   * width8 = width / 8
   */
  int width8;
  
  /**
   * Map height in 8*8 squares.
   * height8 = height / 8
   */
  int height8;

  /**
   * Number of elements in the array obstacle_tiles.
   * obstacle_tiles_size = width8 * height8
   */
  int obstacle_tiles_size;
  
  /**
   * Tileset of the map.
   * Every tile of this map is extracted from this tileset.
   */
  Tileset *tileset;

  /**
   * Id of the background music of the map (can be a valid music, Music::none or Music::unchanged).
   */
  MusicId music_id;

  // entities
  void add_new_tile(int tile_id, Layer layer, int x, int y, int width, int height);
  void add_entrance(string entrance_name, Layer layer, int link_x, int link_y, int link_direction);
  void add_exit(string exit_name, Layer layer, int x, int y, int w, int h,
		Transition transition, MapId map_id, string entrance_name);

 public:

  Map(MapId id);
  ~Map(void);

  inline Tileset *get_tileset(void) { return tileset; }
  SDL_Surface *get_visible_surface(void);
  inline SDL_Rect *get_screen_position(void) { return &screen_position; }

  bool is_loaded(void);
  void load(void);
  void unload(void);

  void start(void);
  void leave(void);

  void set_entrance(unsigned int entrance_index);
  void set_entrance(string entrance_name);

  void update_sprites(void);
  void display();
  void display_sprite(AnimatedSprite *sprite, const SDL_Rect *position_in_map);
  
  Obstacle pixel_collision(int layer, int x, int y);
  bool collision_with_tiles(int layer, SDL_Rect &collision_box);

  void entity_just_moved(MovingWithCollision *entity);

  // Events

  void event_entity_on_detector(EntityDetector *detector, MapEntity *entity);

};

#endif
