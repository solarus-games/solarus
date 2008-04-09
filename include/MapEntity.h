/**
 * This module defines the class MapEntity.
 */

#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include "Common.h"

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
   * relative to the top-left corner.
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

  MapEntity(void);
  MapEntity(Layer layer, int x, int y, int width, int height);
  MapEntity(string name, int direction, Layer layer, int x, int y, int width, int height);

  void set_size(int width, int height);
  void set_hotspot(int x, int y);

 public:

  /**
   * Destructor.
   */
  virtual ~MapEntity(void) { };

  /**
   * Returns the current x position of the entity.
   * @return the x position of the entity
   */
  inline int get_x(void) { return position_in_map.x + hotspot.x; }

  /**
   * Returns the current y position of the entity.
   * @return the y position of the entity
   */
  inline int get_y(void) { return position_in_map.y + hotspot.y; }

  /**
   * Returns the width of the entity.
   * @return the width of the entity
   */
  inline int get_width(void) { return position_in_map.w; }

  /**
   * Returns the height of the entity.
   * @return the height of the entity
   */
  inline int get_height(void) { return position_in_map.h; }

  /**
   * Returns the name of the entity (if any).
   * @return the name of the entity, or an empty string if the entity is not identifiable
   */
  inline string get_name(void) { return name; }

  /**
   * Returns the direction of the entity.
   * @return the direction of the entity
   */
  inline int get_direction(void) { return direction; }

  /**
   * Returns the position of the entity.
   * @return the position of the entity
   */
  inline const SDL_Rect *get_position_in_map(void) { return &position_in_map; }

  /**
   * Returns the layer of the entity on the map.
   * @return the layer of the entity on the map.
   */
  inline Layer get_layer() { return layer; }

  virtual void update();

  /**
   * Displays the entity on the map.
   * This is an abstract function because a tile is not
   * displayed like an animated sprite.
   */
  virtual void display_on_map(Map *map) = 0;

  bool overlaps(const SDL_Rect *rectangle);
  bool is_hotspot_in(const SDL_Rect *rectangle);
};

#endif
