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

  friend class MapLoader; // the map loader modifies the private fields of Map

 private:

  static MapLoader map_loader;  /**< the map file parser */

  // map properties

  MapId id;                     /**< id of the map */
  
  int width8;                   /**< map width in 8*8 squares (width8 = get_width() / 8) */
  int height8;                  /**< map height in 8*8 squares (height8 = get_height() / 8) */

  Tileset *tileset;             /**< tileset of the map: every tile of this map
				 * is extracted from this tileset. */

  MusicId music_id;             /**< id of the background music of the map: 
				 * can be a valid music, Music::none or Music::unchanged */

  int world;                    /**< the world where this map is:
				 * - 0 if the map is in the outside world
				 * - -1 if the map is in the inside world
				 * - 1 to 20 if the map is in a dungeon */

  int floor;                    /**< The floor where this map is:
				 * - -16 to 15 for a normal floor
				 * - -100 to indicate that there is no floor
				 * - -99 for the unknown floor '?' */

  SDL_Rect location;            /**< location of the map in its context: the width and height fields
				 * indicate the map size in pixel, and the x and y field indicate the position:
				 * - in the outside world: location of the map's top-left corner
				 *   relative to the whole world map
				 * - in the inside world: location of the map relative to the whole world map
				 * - in a dungeon: location of the map's top-left corner relative to the whole floor */

  int small_keys_variable;      /**< index of the variable of the savegame where the number of small keys
				 * of this map is saved (-1 means that the small keys are not enabled on this map) */

  // screen

  SDL_Rect screen_position;     /**< position of the screen in the map */
  SDL_Surface *visible_surface; /**< surface where the map is displayed - this surface is only the visible part
				 * of the map, so the coordinates on this surface are relative to the screen,
				 * not to the map */

  // map state
  bool started;                 /**< true if this map is the current map */
  int destination_point_index;  /**< current destination point on the map
				 * (or -1 for "_same", or -2 for "_side") */
  int destination_side;         /**< destination side (when the destination is a side of the map) */

  MapEntities *entities;        /**< the entities on the map */
  bool suspended;               /**< indicates whether the game is suspended */

  MapScript *script;            /**< LUA script of the map */

 public:

  // creation and destruction
  Map(MapId id);
  ~Map(void);

  // map properties
  MapId get_id(void);
  Tileset *get_tileset(void);
  MapScript *get_script(void);
  int get_world_number(void);
  bool is_in_dungeon(void);
  bool is_in_outside_world(void);
  int get_floor(void);
  bool has_floor(void);
  const SDL_Rect *get_location(void);
  int get_small_keys_variable(void);
  bool has_small_keys(void);

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
  void set_suspended(bool suspended);

  // entities
  MapEntities *get_entities(void);

  // Link's presence
  bool is_started(void);
  void start(void);
  void leave(void);

  // current destination point
  void set_destination_point(unsigned int destination_point_index);
  void set_destination_point(string destination_point_name);
  void place_hero_on_destination_point(void);

  // collisions with obstacles (checked before a move)
  MapEntity::Obstacle pixel_collision_with_tiles(MapEntity::Layer layer, int x, int y);
  bool collision_with_entities(MapEntity::Layer layer, SDL_Rect &collision_box, MapEntity *entity_to_check);
  bool collision_with_obstacles(MapEntity::Layer layer, SDL_Rect &collision_box, MapEntity *entity_to_check);
  bool collision_with_obstacles(MapEntity::Layer layer, int x, int y, MapEntity *entity_to_check);

  // collisions with detectors (checked after a move)
  void check_collision_with_detectors(MapEntity *entity);
  void check_collision_with_detectors(MapEntity *entity, Sprite *sprite);

  // update and display
  void update(void);
  void display();
  void display_sprite(Sprite *sprite, int x, int y);
};

#endif
