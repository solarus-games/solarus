#include "MapEntity.h"

/**
 * Creates a map entity without specifying its properties yet.
 */
MapEntity::MapEntity(void):
  layer(LAYER_LOW), name(""), direction(0) {

  position_in_map.x = 0;
  position_in_map.y = 0;

  set_size(0, 0);
  set_hotspot(0, 0);
}

/**
 * Creates a map entity, specifying its position.
 * The entity has no name and no direction.
 * @param layer layer of the entity
 * @param x x position of the entity
 * @param y y position of the entity
 * @param width width of the entity
 * @param height height of the entity
 */
MapEntity::MapEntity(Layer layer, int x, int y, int width, int height):
  layer(layer), name(""), direction(0) {

  position_in_map.x = x;
  position_in_map.y = y;

  set_size(width, height);
  set_hotspot(0, 0);
}

/**
 * Creates an entity, specifying its position, its name and its direction.
 * @param name a name identifying the entity
 * @param direction direction of the entity
 * @param layer layer of the entity
 * @param x x position of the entity
 * @param y y position of the entity
 * @param width width of the entity
 * @param height height of the entity
 */
MapEntity::MapEntity(string name, int direction, Layer layer, int x, int y, int width, int height):
  layer(layer), name(name), direction(direction) {

  position_in_map.x = x;
  position_in_map.y = y;

  set_size(width, height);
  set_hotspot(0, 0);
}

/**
 * Sets the size of the entity.
 * @param width the entity's width
 * @param height the entity's height
 */
void MapEntity::set_size(int width, int height) {
  position_in_map.w = width;
  position_in_map.h = height;
}
/**
 * Sets the hotspot of the entity, i.e. its origin point,
 * relative to the top-left corner.
 * @param x hotspot x
 * @param y hotspot y
 */
void MapEntity::set_hotspot(int x, int y) {
  hotspot.x = x;
  hotspot.y = y;
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

  bool overlap_x = (x3 < x2 && x1 < x4);

  int y1 = position_in_map.y;
  int y2 = y1 + position_in_map.h;
  int y3 = rectangle->y;
  int y4 = y3 + rectangle->h;

  bool overlap_y = (y3 < y2 && y1 < y4);

  return overlap_x && overlap_y;
}

/**
 * Returns whether or not this entity's hotspot is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's hotspot is in the rectangle specified, false otherwise
 */
bool MapEntity::is_hotspot_in(const SDL_Rect *rectangle) {

  return position_in_map.x + hotspot.x >= rectangle->x
    && position_in_map.x + hotspot.x < rectangle->x + rectangle->w
    && position_in_map.y + hotspot.y >= rectangle->y
    && position_in_map.y + hotspot.y < rectangle->y + rectangle->h;
}
