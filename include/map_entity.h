#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include <SDL/SDL.h>

class Map;

/* Abstract class for all objects attached to a map: tiles,
 * enemies, Link, interactive objects, etc.
 * A map entity has a position in the map.
 */
class MapEntity {

 protected:
  /* Position in the map
   */
  SDL_Rect where_in_map;

 public:
  /* Constructor
   */
  inline MapEntity(void) { };

  /* Destructor
   */
  virtual ~MapEntity(void) { };

  /* Return the current x of the entity
   */
  inline int get_x(void) { return where_in_map.x; }

  /* Return the current y of the entity
   */
  inline int get_y(void) { return where_in_map.y; }

  /* Return the width of the entity
   */
  inline int get_width(void) { return where_in_map.w; }

  /* Return the height of the entity
   */
  inline int get_height(void) { return where_in_map.h; }

  /* Display the entity on the map
   * This is an abstract function because a tile is not
   * displayed like an animated sprite.
   */
  virtual void display_on_map(Map *map) = 0;
};

#endif
