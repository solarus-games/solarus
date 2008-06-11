#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include "Common.h"
#include "Layer.h"

/**
 * Abstract class for all objects attached to a map: tiles,
 * enemies, Link, interactive objects, etc.
 * Each entity has a position on the map.
 */
class MapEntity {

 protected:

  /**
   * Position on the map.
   */
  SDL_Rect position_in_map;

  /**
   * Coordinates of the hotspot (i.e. the origin point of the entity),
   * relative to the top-left corner of its rectangle.
   */
  SDL_Rect hotspot;

  /**
   * Layer of the entity: LAYER_LOW, LAYER_INTERMEDIATE or LAYER_HIGH.
   * The layer is constant for the tiles and can change for Link and the enemies.
   */
  Layer layer;

  /**
   * Name of the entity (not used for all kinds of entities).
   */
  string name;

  /**
   * Direction of the entity (not used for all kinds of entities).
   */
  int direction;

  /**
   * Movement of the entity (not used for all kinds of entities).
   */
  Movement *movement;

  MapEntity(void);
  MapEntity(Layer layer, int x, int y, int width, int height);
  MapEntity(string name, int direction, Layer layer, int x, int y, int width, int height);

  void set_size(int width, int height);
  void set_hotspot(int x, int y);
  void set_movement(Movement *movement);
  void clear_movement(void);

 public:

  ~MapEntity(void);

  int get_x(void);
  int get_y(void);
  void set_x(int x);
  void set_y(int y);
  int get_width(void);
  int get_height(void);

  string get_name(void);
  int get_direction(void);
  Layer get_layer(void);
  const SDL_Rect * get_position_in_map(void);

  Movement * get_movement(void);
  virtual void movement_just_changed(void);

  virtual void update();

  bool overlaps(const SDL_Rect *rectangle);
  bool is_hotspot_in(const SDL_Rect *rectangle);

  /**
   * Displays the entity on the map.
   * This is an abstract function because a tile is not
   * displayed like an animated sprite and some entities
   * are invisible.
   */
  virtual void display_on_map(Map *map) = 0;
};

#endif
