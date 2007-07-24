/**
 * This module defines the class MapEntity.
 */

#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include <SDL/SDL.h>

class Map;

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
   * Constructor.
   */
  inline MapEntity(void) { };

 public:

  /**
   * Destructor.
   */
  virtual ~MapEntity(void) { };

  /**
   * Returns the current x position of the entity.
   * @return the x position of the entity
   */
  inline int get_x(void) { return position_in_map.x; }

  /**
   * Returns the current y position of the entity.
   * @return the y position of the entity
   */
  inline int get_y(void) { return position_in_map.y; }

  /**
   * Returns the width of the entity
   * @return the width of the entity
   */
  inline int get_width(void) { return position_in_map.w; }

  /**
   * Returns the height of the entity
   * @return the height of the entity
   */
  inline int get_height(void) { return position_in_map.h; }

  /**
   * Displays the entity on the map.
   * This is an abstract function because a tile is not
   * displayed like an animated sprite.
   */
  virtual void display_on_map(Map *map) = 0;
};

#endif
