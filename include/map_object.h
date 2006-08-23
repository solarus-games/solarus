#ifndef ZSDX_MAP_OBJECT_H
#define ZSDX_MAP_OBJECT_H

#include <SDL/SDL.h>

// TODO: rename into MapEntity

/* Abstract class for all objects attached to a map: tiles,
 * enemies, Link, interactive objects, etc.
 * A map object has a position in the map.
 */
class MapObject {

 protected:
  /* Position in the map
   */
  SDL_Rect where_in_map;

 public:
  /* Constructor
   */
  inline MapObject(void) { };

  /* Destructor
   */
  virtual ~MapObject(void) { };

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
  virtual void display_on_map(SDL_Surface *map) = 0;
};

#endif
