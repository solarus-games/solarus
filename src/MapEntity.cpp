#include "MapEntity.h"
#include "Movement.h"

/**
 * Creates a map entity without specifying its properties yet.
 */
MapEntity::MapEntity(void):
  layer(LAYER_LOW), name(""), direction(0), movement(NULL) {

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
  layer(layer), name(""), direction(0), movement(NULL) {

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
  layer(layer), name(name), direction(direction), movement(NULL) {

  position_in_map.x = x;
  position_in_map.y = y;

  set_size(width, height);
  set_hotspot(0, 0);
}

/**
 * Destructor.
 * The entity's movement is deleted (if any).
 */
MapEntity::~MapEntity(void) {

  if (movement != NULL) {
    clear_movement();
  }
}

/**
 * Returns the current x position of the entity.
 * @return the x position of the entity
 */
int MapEntity::get_x(void) {
  return position_in_map.x + hotspot.x;
}

/**
 * Returns the current y position of the entity.
 * @return the y position of the entity
 */
int MapEntity::get_y(void) {
  return position_in_map.y + hotspot.y;
}

/**
 * Sets the x position of the entity.
 * This function is called by the movement object
 * of this entity.
 * @param x the new x position
 */
void MapEntity::set_x(int x) {
  position_in_map.x = x - hotspot.x;
}

/**
 * Sets the y position of the entity.
 * This function is called by the movement object
 * of this entity.
 * @param y the new y position
 */
void MapEntity::set_y(int y) {
  position_in_map.y = y - hotspot.y;
}

/**
 * Returns the width of the entity.
 * @return the width of the entity
 */
int MapEntity::get_width(void) {
  return position_in_map.w;
}

/**
 * Returns the height of the entity.
 * @return the height of the entity
 */
int MapEntity::get_height(void) {
  return position_in_map.h;
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
 * Returns the name of the entity (if any).
 * @return the name of the entity, or an empty string if the entity is not identifiable
 */
string MapEntity::get_name(void) {
  return name;
}

/**
 * Returns the direction of the entity.
 * @return the direction of the entity
 */
int MapEntity::get_direction(void) {
  return direction;
}

/**
 * Returns the layer of the entity on the map.
 * @return the layer of the entity on the map.
 */
Layer MapEntity::get_layer(void) {
  return layer;
}

/**
 * Returns the position of the entity.
 * @return the position of the entity
 */
const SDL_Rect * MapEntity::get_position_in_map(void) {
  return &position_in_map;
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
 * Returns the current movement of the entity.
 * @return the entity's movement, or NULL if there is no movement
 */
Movement *MapEntity::get_movement(void) {
  return movement;
}

/**
 * Sets the movement of this entity.
 * If a movement was already set, it is not deleted (so that you can reassign
 * it later). If you want to delete it, call clear_movement() first.
 * @param movement the movement to set
 */
void MapEntity::set_movement(Movement *movement) {
  this->movement = movement;
  movement->set_entity(this);
}

/**
 * Removes and deletes the movement of this entity.
 * @param movement the movement to set
 */
void MapEntity::clear_movement(void) {
  delete movement;
  movement = NULL;
}

/**
 * This function is called to notify the entity when its movement
 * has just changed.
 * By default, it does nothing.
 */
void MapEntity::movement_just_changed(void) {

}

/**
 * Updates the entity.
 * This function is called repeteadly by the map. By default, it updates the position
 * of the entity, according to its movement (if any).
 * Redefine it in subclasses for the entities that should be updated
 * for other treatments (such as animations), but don't forget to call this method
 * to handle the movement.
 */
void MapEntity::update(void) {

  if (movement != NULL) {
    movement->update();
  }
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
