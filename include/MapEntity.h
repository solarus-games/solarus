/**
 * This module defines the class MapEntity.
 */

#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include <SDL/SDL.h>
#include "Types.h"

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
   * Layer of the entity: LAYER_LOW, LAYER_INTERMEDIATE or LAYER_HIGH.
   * The layer is constant for the tiles and can change for Link and the enemies.
   */
  Layer layer;

  /**
   * Constructor.
   * @param layer layer of the entity
   */
  inline MapEntity(Layer layer):
    layer(layer) { };

  /**
   * Constructor.
   * @param layer layer of the entity
   * @param x x position of the entity
   * @param y y position of the entity
   * @param w width of the entity
   * @param h height of the entity
   */
  MapEntity(Layer layer, int x, int y, int width, int height);

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
   * Returns the position of the entity.
   * @return the position of the entity
   */
  inline const SDL_Rect *get_position_in_map(void) { return &position_in_map; }

  /**
   * Returns the layer of the entity on the map.
   * @return the layer of the entity on the map.
   */
  inline Layer get_layer() { return layer; }

  /**
   * Displays the entity on the map.
   * This is an abstract function because a tile is not
   * displayed like an animated sprite.
   */
  virtual void display_on_map(Map *map) = 0;

  /**
   * Returns whether or not this entity's rectangle overlaps
   * a specified rectangle.
   * @param rectangle the rectangle to check
   * @return true if this entity's rectangle overlaps the rectangle specified, false otherwise
   */
  bool overlaps(const SDL_Rect *rectangle);
};

#endif
