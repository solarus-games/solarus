#include "entities/MapEntity.h"
#include "entities/MapEntities.h"
#include "movements/Movement.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "Map.h"
#include <cmath>

/**
 * Creates a map entity without specifying its properties yet.
 */
MapEntity::MapEntity(void):
  map(NULL), layer(LAYER_LOW), name(""), direction(0), movement(NULL),
  suspended(false), when_suspended(0) {

  position_in_map.x = 0;
  position_in_map.y = 0;

  origin.x = 0;
  origin.y = 0;

  set_size(0, 0);
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
MapEntity::MapEntity(MapEntity::Layer layer, int x, int y, int width, int height):
  layer(layer), name(""), direction(0), movement(NULL),
  suspended(false), when_suspended(0) {

  position_in_map.x = x;
  position_in_map.y = y;

  origin.x = 0;
  origin.y = 0;

  set_size(width, height);
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
MapEntity::MapEntity(string name, int direction, MapEntity::Layer layer, int x, int y, int width, int height):
  layer(layer), name(name), direction(direction), movement(NULL),
  suspended(false), when_suspended(0), being_removed(false) {

  position_in_map.x = x;
  position_in_map.y = y;

  origin.x = 0;
  origin.y = 0;

  set_size(width, height);
}

/**
 * Destructor.
 * The sprite and the movement of the entity are deleted (if any).
 */
MapEntity::~MapEntity(void) {

  for (unsigned int i = 0; i < sprites.size(); i++) {
    delete sprites[i];
  }

  if (movement != NULL) {
    clear_movement();
  }
}

/**
 * Sets the map where this entity is.
 * @param map the map
 */
void MapEntity::set_map(Map *map) {
  this->map = map;
}

/**
 * Sets a flag indicating that this entity has been added
 * to the list of entities that will be removed from the map
 * and deleted from the memory as soon as possible.
 * When this flag is on, this entity is not considered
 * to be on the map anymore.
 */
void MapEntity::set_being_removed(void) {
  being_removed = true;
}

/**
 * Returns true if this entity is about to be deleted.
 * When this function returns true, the entity is not
 * considered to be on the map anymore.
 * @return true if this entity is about to be deleted
 */
bool MapEntity::is_being_removed(void) {
  return being_removed;
}

/**
 * Returns the layer of the entity on the map.
 * @return the layer of the entity on the map
 */
MapEntity::Layer MapEntity::get_layer(void) {
  return layer;
}

/**
 * Sets the layer of the entity on the map.
 * @param layer the layer of the entity on the map
 */
void MapEntity::set_layer(Layer layer) {
  this->layer = layer;
}

/**
 * Sets the direction property of this entity.
 * @param direction the direction
 */
void MapEntity::set_direction(int direction) {
  this->direction = direction;
}

/**
 * Returns the current x position of the entity.
 * @return the x position of the entity
 */
int MapEntity::get_x(void) {
  return position_in_map.x + origin.x;
}

/**
 * Returns the current y position of the entity.
 * @return the y position of the entity
 */
int MapEntity::get_y(void) {
  return position_in_map.y + origin.y;
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
 * Sets the y position of the entity.
 * This function is called by the movement object
 * of this entity.
 * @param y the new y position
 */
void MapEntity::set_y(int y) {
  position_in_map.y = y - origin.y;
}

/**
 * Returns the x position of the entity's top-left corner.
 * @return the x position of the entity's top-left corner
 */
int MapEntity::get_top_left_x(void) {
  return position_in_map.x;
}

/**
 * Returns the y position of the entity's top-left corner.
 * @return the y position of the entity's top-left corner
 */
int MapEntity::get_top_left_y(void) {
  return position_in_map.y;
}

/**
 * Sets the x position of the entity's top-left corner.
 * @param x the new top-left x position
 */
void MapEntity::set_top_left_x(int x) {
  position_in_map.x = x;
}

/**
 * Sets the y position of the entity's top-left corner.
 * @param y the new top-left y position
 */
void MapEntity::set_top_left_y(int y) {
  position_in_map.y = y;
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
 * Sets the size of the entity.
 * @param size the width and the height
 */
void MapEntity::set_size(SDL_Rect &size) {
  position_in_map.w = size.w;
  position_in_map.h = size.h;
}

/**
 * Returns the position of the entity.
 * This function returns the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 * @return the position of the entity
 */
const SDL_Rect * MapEntity::get_position_in_map(void) {
  return &position_in_map;
}

/**
 * Sets the position of the entity.
 * This function sets the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 * @param position_in_map the position of the entity
 */
void MapEntity::set_position_in_map(const SDL_Rect *position_in_map) {
  this->position_in_map = *position_in_map;
}

/**
 * Sets the position of the entity.
 * This function sets the position of the rectangle's top-left corner.
 * @param x x position of the entity
 * @param y y position of the entity
 */
void MapEntity::set_position_in_map(int x, int y) {
  set_x(x);
  set_y(y);
}

/**
 * Returns the coordinates of the point the entity is looking at.
 * You should redefine this method to define a facing point.
 * @return the coordinates of the point the entity is looking at
 */
SDL_Rect MapEntity::get_facing_point(void) {
  SDL_Rect point = {-1, -1};
  return point;
}

/**
 * Returns the coordinates of the point the entity would be facing
 * if it was looking towards the specified direction.
 * You should redefine this method to define a facing point.
 * @param direction a direction (0 to 3)
 * @return the coordinates of the point the entity is looking at
 */
SDL_Rect MapEntity::get_facing_point(int direction) {
  SDL_Rect point = {-1, -1};
  return point;
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
 * Returns whether this entity is the hero
 * controlled by the player.
 * @return true if this entity is the hero
 */
bool MapEntity::is_hero(void) {
  return get_type() == HERO;
}

/**
 * Returns the origin point of the entity.
 * @return the origin point
 */
const SDL_Rect * MapEntity::get_origin(void) {
  return &origin;
}

/**
 * Sets the origin point of the entity
 * relative to the top-left corner of its rectangle.
 * @param x x coordinate of the origin
 * @param y y coordinate of the origin
 */
void MapEntity::set_origin(int x, int y) {

  position_in_map.x -= (x - origin.x);
  position_in_map.y -= (y - origin.y);

  origin.x = x;
  origin.y = y;
}

/**
 * Sets the origin point of the entity
 * relative to the top-left corner of its rectangle.
 * @param origin x and y coordinates of the origin
 */
void MapEntity::set_origin(const SDL_Rect *origin) {

  set_origin(origin->x, origin->y);
}

/**
 * Sets the origin point and the size of the entity
 * like its sprite.
 * You should call this function only if the entity's rectangle
 * is the same as the sprite's rectangle.
 * Otherwise, you have to call set_size() and set_origin()
 * explicitely.
 */
void MapEntity::set_rectangle_from_sprite(void) {

  Sprite *sprite = sprites[0];
  set_size(sprite->get_size());
  set_origin(&sprite->get_origin());
}

/**
 * Returns a sprite of the entity.
 * @return the sprite at this index
 */
Sprite * MapEntity::get_sprite(int index) {
  return sprites[index];
}

/**
 * Returns whether the entity has at least one sprite.
 * @return true if the entity has at least one sprite.
 */
bool MapEntity::has_sprite(void) {
  return sprites.size() != 0;
}

/**
 * Returns the sprite created with the last call to create_sprite()
 * for this entity.
 * @return the last sprite created
 */
Sprite * MapEntity::get_sprite(void) {
  return sprites.back();
}

/**
 * Adds a sprite to this entity.
 * @param id id of the sprite's animations to add
 */
void MapEntity::create_sprite(SpriteAnimationSetId id) {
  sprites.push_back(new Sprite(id));
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

    if (movement->is_suspended() != suspended) {
      movement->set_suspended(suspended);
    }
  }
}

/**
 * Removes and deletes the movement of this entity.
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
 * This function is called when the entity has just moved.
 * It just calls map->check_collision_with_detectors(this).
 */
void MapEntity::just_moved(void) {
  map->check_collision_with_detectors(this);
}

/**
 * Sets the entity this entity is currently facing.
 * This function is called when this entity is just being
 * facing another entity.
 * By default, nothing is done.
 */
void MapEntity::set_facing_entity(Detector *detector) {
  
}

/**
 * Returns whether this entity is an obstacle for another one.
 * By default, this function returns false.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool MapEntity::is_obstacle_for(MapEntity *other) {
  return false;
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
 * Returns whether the specified point is inside a rectangle.
 * @param rectangle the rectangle
 * @param x x coordinate of the point
 * @param y y coordinate of the point
 */
bool MapEntity::is_point_in(const SDL_Rect *rectangle, int x, int y) {

  return x >= rectangle->x && x < rectangle->x + rectangle->w
    && y >= rectangle->y && y < rectangle->y + rectangle->h;
}

/**
 * Returns whether or not this entity's origin point is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's origin point is in the rectangle specified, false otherwise
 */
bool MapEntity::is_origin_point_in(const SDL_Rect *rectangle) {

  return is_point_in(rectangle, get_x(), get_y());
}

/**
 * Returns whether or not this entity's facing point is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's facing point is in the rectangle specified, false otherwise
 */
bool MapEntity::is_facing_point_in(const SDL_Rect *rectangle) {

  SDL_Rect facing_point = get_facing_point();
  return is_point_in(rectangle, facing_point.x, facing_point.y);
}

/**
 * Returns whether or not this entity's center is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's center is in the rectangle specified, false otherwise
 */
bool MapEntity::is_center_in(const SDL_Rect *rectangle) {

  return is_point_in(rectangle, get_top_left_x() + get_width() / 2, get_top_left_y() + get_height() / 2);
}

/**
 * Returns the angle of the vector between the origin of this entity
 * and the origin of another entity.
 * @param other the other entity
 * @return the angle of the vector in radians
 */
double MapEntity::get_vector_angle(MapEntity *other) {

  int dx = other->get_x() - get_x();
  int dy = other->get_y() - get_y();

  double angle;
  if (dx != 0) {
    angle = atan((double) -dy / (double) dx);

    if (dx < 0) {
      angle += PI;
    }
  }
  else {
    // special case (cannot divide by zero and compute atan)
    angle = (dy > 0) ? -PI_OVER_2 : PI_OVER_2;
  }

  return angle;
}

/**
 * This function is called when an enemy detects a collision with this entity.
 * @param enemy the enemy
 */
void MapEntity::collision_with_enemy(Enemy *enemy) {
  // nothing done by default
}

/**
 * This function is called when an enemy's sprite detects a collision with this entity.
 * @param enemy the enemy
 * @param sprite_overlapping the sprite of this entity that collides with the enemy
 */
void MapEntity::collision_with_enemy(Enemy *enemy, Sprite *sprite_overlapping) {
  // nothing done by default
}

/**
 * This function is called when a teletransporter detects a collision with this entity.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void MapEntity::collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode) {
  // nothing done by default
}

/**
 * Returns whether the movement and the animations of this entity are suspended.
 * @return true if the movement and the animations are suspended
 */
bool MapEntity::is_suspended(void) {
  return suspended;
}

/**
 * Suspends or resumes the movement and the animations of this entity.
 * This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the movement and the animations, false to resume them
 */
void MapEntity::set_suspended(bool suspended) {

  this->suspended = suspended;

  // remember the date if the movement is being suspended
  if (suspended) {
    when_suspended = SDL_GetTicks();
  }

  // suspend/unsuspend the sprites animations
  for (unsigned int i = 0; i < sprites.size(); i++) {
    Sprite *sprite = sprites[i];
    sprite->set_suspended(suspended);
  }

  // suspend/unsuspend the movement
  if (movement != NULL) {
    movement->set_suspended(suspended);
  }
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
  for (unsigned int i = 0; i < sprites.size(); i++) {

    Sprite *sprite = sprites[i];

    sprite->update();
    if (sprite->has_frame_changed() && sprite->get_animation_set()->are_pixel_collisions_enabled()) {
      map->check_collision_with_detectors(this, sprite);
    }
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
void MapEntity::display_on_map(void) {

  // display the sprites
  for (unsigned int i = 0; i < sprites.size(); i++) {
    map->display_sprite(sprites[i], get_x(), get_y());
  }
}
