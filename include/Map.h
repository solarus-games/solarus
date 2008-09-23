#ifndef ZSDX_MAP_H
#define ZSDX_MAP_H

#include "Common.h"
#include "Transition.h"
#include "entities/PickableItem.h"
#include "movements/MovementFalling.h"

/**
 * Represents a map.
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
   * The map file parser.
   */
  static MapLoader map_loader;
  friend class MapLoader; // the map loader modifies the private fields of Map

  // map properties

  /**
   * Id of the map.
   */
  MapId id;

  /**
   * True if this is the current map.
   */
  bool started;

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
   * Tileset of the map.
   * Every tile of this map is extracted from this tileset.
   */
  Tileset *tileset;

  /**
   * Id of the background music of the map (can be a valid music, Music::none or Music::unchanged).
   */
  MusicId music_id;

  /**
   * The world where this map is:
   * - 0 if the map is outside
   * - -1 if the map is inside
   * - 1 to 20 if the map is in a dungeon 
   */
  int world;

  // screen

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
   * The entities of the map.
   */
  MapEntities *entities;

  /**
   * Current entrance of the map.
   */
  unsigned int entrance_index;

  /**
   * Indicates whether the game is suspended.
   */
  bool suspended;

  void set_suspended(bool suspended);

 public:

  // creation and destruction
  Map(MapId id);
  ~Map(void);

  // map properties
  MapId get_id(void);
  Tileset *get_tileset(void);
  int get_world_number(void);
  bool is_in_dungeon(void);

  int get_width(void);
  int get_height(void);
  int get_width8(void);
  int get_height8(void);

  // screen
  SDL_Surface *get_visible_surface(void);
  SDL_Rect *get_screen_position(void);

  // loading
  bool is_loaded(void);
  void load(void);
  void unload(void);

  // entities
  MapEntities *get_entities(void);

  // Link's presence
  bool is_started(void);
  void start(void);
  void leave(void);

  // current entrance
  void set_entrance(unsigned int entrance_index);
  void set_entrance(string entrance_name);

  // collisions with obstacles (checked before a move)
  MapEntity::Obstacle pixel_collision_with_tiles(MapEntity::Layer layer, int x, int y);
  bool collision_with_entities(MapEntity::Layer layer, SDL_Rect &collision_box);
  bool collision_with_obstacles(MapEntity::Layer layer, SDL_Rect &collision_box);

  // collisions with detectors (checked after a move)
  void check_collision_with_detectors(MapEntity *entity);

  // update and display
  void update(void);
  void display();
  void display_sprite(Sprite *sprite, int x, int y);

  // events
  void event_entity_on_detector(EntityDetector *detector, MapEntity *entity);

};

#endif
