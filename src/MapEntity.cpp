#include "MapEntity.h"
#include "Movement.h"
#include "Sprite.h"
#include "Map.h"

/**
 * Creates a map entity without specifying its properties yet.
 */
MapEntity::MapEntity(void):
  layer(LAYER_LOW), name(""), direction(0), movement(NULL) {

  position_in_map.x = 0;
  position_in_map.y = 0;

  set_size(0, 0);
  set_origin(0, 0);

  sprites = new vector<Sprite*>();
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
  set_origin(0, 0);

  sprites = new vector<Sprite*>();
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
  set_origin(0, 0);

  sprites = new vector<Sprite*>();
}

/**
 * Destructor.
 * The sprite and the movement of the entity are deleted (if any).
 */
MapEntity::~MapEntity(void) {

  for (unsigned int i = 0; i < sprites->size(); i++) {
    delete sprites->at(i);
  }
  delete sprites;

  if (movement != NULL) {
    clear_movement();
  }
}

/**
 * Returns the layer of the entity on the map.
 * @return the layer of the entity on the map.
 */
Layer MapEntity::get_layer(void) {
  return layer;
}

/**
 * Returns the current x position of the entity.
 * @return the x position of the entity
 */
int MapEntity::get_x(void) {
  return position_in_map.x + origin.x;
}

/**
 * Sets the x position of the entity.
 * This function is called by the movement object
 * of this entity.
 * @param x the new x position
 */
void MapEntity::set_x(int x) {
  position_in_map.x = x - origin.x;
}

/**
 * Returns the current y position of the entity.
 * @return the y position of the entity
 */
int MapEntity::get_y(void) {
  return position_in_map.y + origin.y;
}

/**
 * Sets the y position of the entity.
 * This function is called by the movement object
 * of this entity.
 * @param y the new y position
 */
void MapEntity::set_y(int y) {
  position_in_map.y = y - origin.y;
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
 * Returns the position of the entity.
 * This function returns the rectangle defined by
 * get_x(), get_y(), get_width() and get_height().
 * @return the position of the entity
 */
const SDL_Rect * MapEntity::get_position_in_map(void) {
  return &position_in_map;
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
 * Sets the origin point of the entity
 * relative to the top-left corner of its rectangle.
 * @param x x coordinate of the origin
 * @param y y coordinate of the origin
 */
void MapEntity::set_origin(int x, int y) {
  origin.x = x;
  origin.y = y;
}

/**
 * Returns a sprite of the entity.
 * @return the sprite at this index
 */
Sprite * MapEntity::get_sprite(int index) {
  return sprites->at(index);
}

/**
 * Returns the sprite created with the last call to create_sprite()
 * for this entity.
 * @return the last sprite created
 */
Sprite * MapEntity::get_last_sprite(void) {
  return sprites->back();
}

/**
 * Adds a sprite to this entity.
 * @param id id of the sprite's animations to add
 */
void MapEntity::create_sprite(SpriteAnimationsId id) {
  sprites->push_back(new Sprite(id));
}

/**
 * Returns the current movement of the entity.
 * @return the entity's movement, or NULL if there is no movement
 */
Movement * MapEntity::get_movement(void) {
  return movement;
}

/**
 * Sets the movement of this entity.
 * If a movement was already set, it is not deleted (so that you can reassign
 * it later). If you want to delete it, call clear_movement() first.
 * @param movement the movement to set, or NULL to set no movement
 */
void MapEntity::set_movement(Movement *movement) {
  this->movement = movement;

  if (movement != NULL) {
    movement->set_entity(this);
  }
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
 * This function is called by the movement object
 * to notify the entity when its movement has just changed.
 * By default, it does nothing.
 */
void MapEntity::movement_just_changed(void) {

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
 * Returns whether or not this entity's origin point is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's origin point is in the rectangle specified, false otherwise
 */
bool MapEntity::is_origin_point_in(const SDL_Rect *rectangle) {

  return position_in_map.x + origin.x >= rectangle->x
    && position_in_map.x + origin.x < rectangle->x + rectangle->w
    && position_in_map.y + origin.y >= rectangle->y
    && position_in_map.y + origin.y < rectangle->y + rectangle->h;
}

/**
 * Updates the entity.
 * This function is called repeteadly by the map. By default, it updates the position
 * of the entity according to its movement (if any), and it updates the sprites frames
 * (if there are sprites) according to the direction.
 * Redefine it in subclasses for the entities that should be updated
 * for other treatments but don't forget to call this method
 * to handle the movement and the sprites.
 */
void MapEntity::update(void) {

  // update the sprites
  for (unsigned int i = 0; i < sprites->size(); i++) {

    Sprite *sprite = sprites->at(i);

    if (sprite->get_current_direction() != direction) {
      sprite->set_current_direction(direction);
    }

    sprite->update_current_frame();
  }

  // update the movement
  if (movement != NULL) {
    movement->update();
  }
}

/**
 * Displays the entity on the map.
 * By default, this function displays the entity's sprites (if any).
 */
void MapEntity::display_on_map(Map *map) {

  // display the sprites
  for (unsigned int i = 0; i < sprites->size(); i++) {
    map->display_sprite(sprites->at(i), get_x(), get_y());
  }
}
