#include "MapEntity.h"

/**
 * Constructor.
 * @param layer layer of the entity
 * @param x x position of the entity
 * @param y y position of the entity
 * @param w width of the entity
 * @param h height of the entity
 */
MapEntity::MapEntity(Layer layer, int x, int y, int width, int height):
  layer(layer) {

  position_in_map.x = x;
  position_in_map.y = y;
  position_in_map.w = width;
  position_in_map.h = height;
}

/**
 * Returns whether or not this entity's rectangle overlaps
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's rectangle overlaps the rectangle specified, false otherwise
 */
bool MapEntity::overlaps(const SDL_Rect *rectangle) {

  int x1 = position_in_map.x;
  int x2 = x1 + position_in_map.w;
  int x3 = rectangle->x;
  int x4 = x3 + rectangle->w;

  bool overlap_x = (x3 <= x2 && x1 <= x4);

  int y1 = position_in_map.y;
  int y2 = y1 + position_in_map.h;
  int y3 = rectangle->y;
  int y4 = y3 + rectangle->h;

  bool overlap_y = (y3 <= y2 && y1 <= y4);

  return overlap_x && overlap_y;
}
