/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 *
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/MapEntity.h"
#include "entities/MapEntities.h"
#include "entities/Tileset.h"
#include "entities/Tile.h"
#include "entities/Teletransporter.h"
#include "entities/DestinationPoint.h"
#include "entities/PickableItem.h"
#include "entities/DestructibleItem.h"
#include "entities/Chest.h"
#include "entities/JumpSensor.h"
#include "entities/Enemy.h"
#include "entities/InteractiveEntity.h"
#include "entities/Block.h"
#include "entities/DynamicTile.h"
#include "entities/Switch.h"
#include "entities/CustomObstacle.h"
#include "entities/Sensor.h"
#include "entities/CrystalSwitch.h"
#include "entities/CrystalSwitchBlock.h"
#include "entities/ShopItem.h"
#include "entities/ConveyorBelt.h"
#include "entities/Door.h"
#include "movements/Movement.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "Map.h"
#include "Geometry.h"

const MapEntity::CreationFunction* MapEntity::creation_functions[] = {
  Tile::parse,
  DestinationPoint::parse,
  Teletransporter::parse,
  PickableItem::parse,
  DestructibleItem::parse,
  Chest::parse,
  JumpSensor::parse,
  Enemy::parse,
  InteractiveEntity::parse,
  Block::parse,
  DynamicTile::parse,
  Switch::parse,
  CustomObstacle::parse,
  Sensor::parse,
  CrystalSwitch::parse,
  CrystalSwitchBlock::parse,
  ShopItem::parse,
  ConveyorBelt::parse,
  Door::parse,
};


const MapEntity::EntityTypeFeatures MapEntity::entity_types_features[] = {
  // can_be_obstacle, can_detect_entities, can_be_displayed, is_displayed_in_y_order
  {false, false, false, false}, // tile (not used)
  {false, false,  true, false}, // destination point
  { true,  true,  true, false}, // teletransporter
  {false,  true,  true, false}, // pickable item
  { true,  true,  true, false}, // destructible item
  { true,  true,  true,  true}, // chest
  { true,  true, false, false}, // jump sensor
  { true,  true,  true,  true}, // enemy
  { true,  true,  true,  true}, // interactive entity
  { true,  true,  true,  true}, // block
  { true, false,  true, false}, // dynamic tile
  {false,  true,  true, false}, // switch
  { true, false, false, false}, // custom obstacle
  { true,  true, false, false}, // sensor
  { true,  true,  true, false}, // crystal switch
  { true,  true,  true, false}, // crystal switch block
  { true,  true,  true, false}, // shop item
  { true,  true,  true, false}, // conveyor belt
  { true,  true,  true, false}, // door
};

/**
 * Creates a map entity without specifying its properties yet.
 */
MapEntity::MapEntity(void):
  map(NULL), layer(LAYER_LOW), name(""), direction(0), movement(NULL),
  suspended(false), when_suspended(0), being_removed(false) {

  rectangle.x = 0;
  rectangle.y = 0;

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
MapEntity::MapEntity(Layer layer, int x, int y, int width, int height):
  map(NULL), layer(layer), name(""), direction(0), movement(NULL),
  suspended(false), when_suspended(0), being_removed(false) {

  rectangle.x = x;
  rectangle.y = y;

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
MapEntity::MapEntity(const std::string &name, int direction, Layer layer,
		     int x, int y, int width, int height):
  map(NULL), layer(layer), name(name), direction(direction), movement(NULL),
  suspended(false), when_suspended(0), being_removed(false) {

  rectangle.x = x;
  rectangle.y = y;

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
 * Returns whether this entity is the hero
 * controlled by the player.
 * @return true if this entity is the hero
 */
bool MapEntity::is_hero(void) {
  return get_type() == HERO;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool MapEntity::can_be_obstacle(void) {
  return entity_types_features[get_type()].can_be_obstacle;
}

/**
 * Returns whether entities of this type can detect the presence
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function
 * notify_collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool MapEntity::can_detect_entities(void) {
  return entity_types_features[get_type()].can_detect_entities;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool MapEntity::can_be_displayed(void) {
  return entity_types_features[get_type()].can_be_displayed;
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file),
 * and before the entities with the feature.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool MapEntity::is_displayed_in_y_order(void) {
  return entity_types_features[get_type()].is_displayed_in_y_order;
}

/**
 * Sets the map where this entity is.
 * @param map the map
 */
void MapEntity::set_map(Map *map) {
  this->map = map;

  // notify the sprites (useful for tileset dependent sprites such as doors and blocks)
  std::vector<Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {
    (*it)->get_animation_set()->set_map(map);
  }
}

/**
 * Returns the map where this entity is.
 * @return the map
 */
Map * MapEntity::get_map(void) {
  return map;
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
Layer MapEntity::get_layer(void) {
  return layer;
}

/**
 * Sets the layer of the entity on the map.
 * If the entity is stored on the map in the class MapEntities,
 * you should not call this function directly: call
 * MapEntities::set_entity_layer() instead because the class MapEntities
 * stores different lists of entities for each layer.
 * @param layer the layer of the entity on the map
 */
void MapEntity::set_layer(Layer layer) {
  this->layer = layer;
}

/**
 * Returns the direction of the entity.
 * @return the direction of the entity
 */
int MapEntity::get_direction(void) {
  return direction;
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
  return rectangle.x + origin.x;
}

/**
 * Returns the current y position of the entity.
 * @return the y position of the entity
 */
int MapEntity::get_y(void) {
  return rectangle.y + origin.y;
}

/**
 * Sets the x position of the entity.
 * This function is called by the movement object
 * of this entity.
 * @param x the new x position
 */
void MapEntity::set_x(int x) {
  rectangle.x = x - origin.x;
}

/**
 * Sets the y position of the entity.
 * This function is called by the movement object
 * of this entity.
 * @param y the new y position
 */
void MapEntity::set_y(int y) {
  rectangle.y = y - origin.y;
}

/**
 * Returns the coordinates of the origin point of the entity, relative to the map.
 * These are the coordinates of the point as returned by get_x() and get_y().
 * @return the coordinates of the entity on the map
 */
const SDL_Rect MapEntity::get_xy(void) {
  SDL_Rect coords = {get_x(), get_y()};
  return coords;
}

/**
 * Sets the coordinates of the origin point of the entity, relative to the map.
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 * @param x the new x coordinate of the entity on the map
 * @param y the new y coordinate of the entity on the map
 */
void MapEntity::set_xy(int x, int y) {
  set_x(x);
  set_y(y);
}

/**
 * Sets the coordinates of the origin point of the entity, relative to the map.
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 * @param coordinates the new coordinates of the entity on the map
 */
void MapEntity::set_xy(const SDL_Rect &xy) {
  set_xy(xy.x, xy.y);
}

/**
 * Returns the x position of the entity's top-left corner.
 * @return the x position of the entity's top-left corner
 */
int MapEntity::get_top_left_x(void) {
  return rectangle.x;
}

/**
 * Returns the y position of the entity's top-left corner.
 * @return the y position of the entity's top-left corner
 */
int MapEntity::get_top_left_y(void) {
  return rectangle.y;
}

/**
 * Sets the x position of the entity's top-left corner.
 * @param x the new top-left x position
 */
void MapEntity::set_top_left_x(int x) {
  rectangle.x = x;
}

/**
 * Sets the y position of the entity's top-left corner.
 * @param y the new top-left y position
 */
void MapEntity::set_top_left_y(int y) {
  rectangle.y = y;
}

/**
 * Sets the position of the entity.
 * This function sets the coordinates of the rectangle's top-left corner.
 * @param x x position of the entity
 * @param y y position of the entity
 */
void MapEntity::set_top_left_xy(int x, int y) {
  set_top_left_x(x);
  set_top_left_y(y);
}

/**
 * Returns the width of the entity.
 * @return the width of the entity
 */
int MapEntity::get_width(void) {
  return rectangle.w;
}

/**
 * Returns the height of the entity.
 * @return the height of the entity
 */
int MapEntity::get_height(void) {
  return rectangle.h;
}

/**
 * Sets the size of the entity.
 * @param width the entity's width
 * @param height the entity's height
 */
void MapEntity::set_size(int width, int height) {
  rectangle.w = width;
  rectangle.h = height;
}

/**
 * Sets the size of the entity.
 * @param size the width and the height
 */
void MapEntity::set_size(SDL_Rect &size) {
  rectangle.w = size.w;
  rectangle.h = size.h;
}

/**
 * Returns the position of the entity.
 * This function returns the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 * @return the position of the entity
 */
const SDL_Rect &MapEntity::get_rectangle(void) {
  return rectangle;
}

/**
 * Sets the position and size of the entity.
 * This function sets the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 * @param rectangle the position of the entity
 */
void MapEntity::set_rectangle(const SDL_Rect &rectangle) {
  this->rectangle = rectangle;
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
  set_origin(sprite->get_origin());
}

/**
 * Returns whether the entity's rectangle is aligned with the 8*8 grid of the map.
 * @return true if the entity's rectangle is aligned
 */
bool MapEntity::is_aligned_to_grid(void) {

  int x = get_top_left_x();
  int y = get_top_left_y();

  return x % 8 == 0 && y % 8 == 0;
}

/**
 * Returns the coordinates of the point the entity is looking at.
 * You should redefine this method to define a facing point.
 * @return the coordinates of the point the entity is looking at
 */
const SDL_Rect MapEntity::get_facing_point(void) {
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
const SDL_Rect MapEntity::get_facing_point(int direction) {
  SDL_Rect point = {-1, -1};
  return point;
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
 * Returns the coordinates of the center point of the entity's rectangle.
 * @return the coordinates of the center point of the entity
 */
const SDL_Rect MapEntity::get_center_point(void) {
  SDL_Rect center = {get_top_left_x() + get_width() / 2, get_top_left_y() + get_height() / 2};
  return center;
}

/**
 * Returns the name of the entity (if any).
 * @return the name of the entity, or an empty string if the entity is not identifiable
 */
const std::string& MapEntity::get_name(void) const {
  return name;
}

/**
 * Returns whether the name of this entity starts with the specified prefix.
 * @param prefix a prefix
 * @return true if the name starts with this prefix
 */
bool MapEntity::has_prefix(const std::string &prefix) {
  return name.substr(0, prefix.size()) == prefix;
}

/**
 * Returns the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * @return the origin point
 */
const SDL_Rect & MapEntity::get_origin(void) {
  return origin;
}

/**
 * Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * @param x x coordinate of the origin
 * @param y y coordinate of the origin
 */
void MapEntity::set_origin(int x, int y) {

  rectangle.x -= (x - origin.x);
  rectangle.y -= (y - origin.y);

  origin.x = x;
  origin.y = y;
}

/**
 * Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * @param origin x and y coordinates of the origin
 */
void MapEntity::set_origin(const SDL_Rect &origin) {

  set_origin(origin.x, origin.y);
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
void MapEntity::create_sprite(const SpriteAnimationSetId &id) {
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
 * This function can be called by the movement object
 * to notify the entity when its movement has just changed.
 * By default, nothing is done.
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
 * Returns whether this entity is an obstacle for another one.
 * By default, this function returns false.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool MapEntity::is_obstacle_for(MapEntity *other) {
  return false;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool MapEntity::is_water_obstacle(void) {
  return true;
}

/**
 * Returns whether a hole is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the holes are currently an obstacle for this entity
 */
bool MapEntity::is_hole_obstacle(void) {
  return true;
}

/**
 * Returns whether a ladder is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the ladders are currently an obstacle for this entity
 */
bool MapEntity::is_ladder_obstacle(void) {
  return true;
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool MapEntity::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return true;
}

/**
 * Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool MapEntity::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return true;
}

/**
 * Returns whether a sensor is currently considered as an obstacle for this entity.
 * This function returns false by default.
 * @param sensor a sensor
 * @return true if the sensor are currently an obstacle for this entity
 */
bool MapEntity::is_sensor_obstacle(Sensor *sensor) {
  return false;
}

/**
 * Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param raised_block a crystal switch block raised
 * @return true if the raised block is currently an obstacle for this entity
 */
bool MapEntity::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  return true;
}

/**
 * Returns whether a crystal switch is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * It should be redefined only for entities that can activate a crystal switch: a pot, the boomerang, etc.
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool MapEntity::is_crystal_switch_obstacle(CrystalSwitch *crystal_switch) {
  return true;
}

/**
 * Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param npc a non-playing character
 * @return true if the NPC is currently an obstacle for this entity
 */
bool MapEntity::is_npc_obstacle(InteractiveEntity *npc) {
  return true;
}

/**
 * Returns whether a jump sensor is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool MapEntity::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return true;
}

/**
 * Returns true if this entity does not react to the sword.
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 * @return true if the sword is ignored
 */
bool MapEntity::is_sword_ignored(void) {
  return false;
}

/**
 * Returns whether or not this entity's rectangle overlaps
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's rectangle overlaps the rectangle specified, false otherwise
 */
bool MapEntity::overlaps(const SDL_Rect &rectangle) {
  return Geometry::overlaps(get_rectangle(), rectangle);
}

/**
 * Returns whether or not a point overlaps this entity's rectangle.
 * @param x x coordinate of the point to check
 * @param y y coordinate of the point to check
 * @return true if the point is in this entity's rectangle
 */
bool MapEntity::overlaps(int x, int y) {
  return Geometry::is_point_in(get_rectangle(), x, y);
}

/**
 * Returns whether or not this entity's rectangle overlaps
 * another entity's rectangle.
 * @param other another entity
 * @return true if this entity's rectangle overlaps the other entity's rectangle
 */
bool MapEntity::overlaps(MapEntity *other) {
  return overlaps(other->get_rectangle());
}

/**
 * Returns whether or not this entity's origin point is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's origin point is in the rectangle specified, false otherwise
 */
bool MapEntity::is_origin_point_in(const SDL_Rect &rectangle) {
  return Geometry::is_point_in(rectangle, get_x(), get_y());
}

/**
 * Returns whether or not this entity's facing point is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's facing point is in the rectangle specified, false otherwise
 */
bool MapEntity::is_facing_point_in(const SDL_Rect &rectangle) {

  SDL_Rect facing_point = get_facing_point();
  return Geometry::is_point_in(rectangle, facing_point.x, facing_point.y);
}

/**
 * Returns whether or not this entity's center is in
 * a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's center is in the rectangle specified, false otherwise
 */
bool MapEntity::is_center_in(const SDL_Rect &rectangle) {

  const SDL_Rect &center = get_center_point();
  return Geometry::is_point_in(rectangle, center.x, center.y);
}

/**
 * Returns the angle of the vector between the origin of this entity
 * and the origin of another entity.
 * @param other the other entity
 * @return the angle of the vector in radians
 */
double MapEntity::get_vector_angle(MapEntity *other) {
  return Geometry::get_angle(get_x(), get_y(), other->get_x(), other->get_y());
}

/**
 * Returns the distance between the origin of this entity and a point.
 * @param x x coordinate of the point
 * @param y y coordinate of the point
 * @return the distance between this entity and the point in pixels
 */
int MapEntity::get_distance(int x, int y) {
  return (int) Geometry::get_distance(get_x(), get_y(), x, y);
}

/**
 * Returns the distance between the origin of this entity
 * and the origin of another entity.
 * @param other the other entity
 * @return the distance between the two entities in pixels
 */
int MapEntity::get_distance(MapEntity *other) {
  return (int) Geometry::get_distance(get_x(), get_y(), other->get_x(), other->get_y());
}

/*
 * Tries to make sure this entity is not overlapping the obstacles of the map.
 * This function should be used only in very specific situations, when there is no way to avoid the possibility
 * that the entity can be on an obstacle. Normally, the movement of an entity is such that
 * the entity never overlaps an obstacle.
 * If the entity is on an obstacle, we try to move it in the eight possible direction up to 8 pixels away,
 * until an obstacle-free position is found.
 * If no free position is found, the entity is not moved.
 */
/*
void MapEntity::ensure_no_obstacles(void) {

  SDL_Rect collision_box = get_rectangle();

  if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
    return;
  }

  bool found = false;

  static const int dx[] = { 1, 0, -1, 0, 1, -1, -1, 1 };
  static const int dy[] = { 0, -1, 0, 1, -1, -1, 1, 1 };

  for (int i = 0; i < 12 && !found; i++) {
    for (int j = 0; j < 8 && !found; j++) {
      collision_box.x += dx[j] * i;
      collision_box.y += dy[j] * i;

      if (!map->test_collision_with_obstacles(get_layer(), collision_box, this)) {
	found = true;
	set_rectangle(collision_box);
	just_moved();
      }

      collision_box.x -= dx[j] * i;
      collision_box.y -= dy[j] * i;
    }
  }
}
*/

/**
 * This function is called when an enemy's rectangle detects a collision with this entity's rectangle.
 * @param enemy the enemy
 */
void MapEntity::notify_collision_with_enemy(Enemy *enemy) {
  // nothing done by default
}

/**
 * This function is called when a teletransporter detects a collision with this entity.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode) {
  // nothing done by default
}

/**
 * This function is called when a conveyor belt detects a collision with this entity.
 * @param conveyor_belt a conveyor belt
 * @param dx direction of the x move in pixels (0, 1 or -1)
 * @param dy direction of the y move in pixels (0, 1 or -1)
 */
void MapEntity::notify_collision_with_conveyor_belt(ConveyorBelt *conveyor_belt, int dx, int dy) {
  // nothing done by default
}

/**
 * This function is called when a sensor detects a collision with this entity.
 * @param sensor a sensor
 */
void MapEntity::notify_collision_with_sensor(Sensor *sensor) {
  // nothing done by default
}

/**
 * This function is called when an explosion's sprite detects a collision with this entity's sprite.
 * @param explosion the explosion
 */
void MapEntity::notify_collision_with_explosion(Explosion *explosion) {
  // nothing done by default
}

/**
 * This function is called when an enemy's sprite detects a pixel-perfect collision with this entity.
 * @param enemy the enemy
 * @param sprite_overlapping the sprite of this entity that collides with the enemy
 */
void MapEntity::notify_collision_with_enemy(Enemy *enemy, Sprite *sprite_overlapping) {
  // nothing done by default
}

/**
 * Notifies this entity that it has just attacked an enemy
 * (even if this attack was not successful).
 * @param attack the attack
 * @param victim the enemy just attacked
 * @param result indicates how the enemy reacted to the attack:
 * - a number greater than 0 represents the number of health points the enemy has just lost
 * - a value of 0 means that the attack was ignored
 * - a value of -1 means that the enemy was protected against the attack
 * - a value of -2 means that the attack immobilized the enemy
 */
void MapEntity::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result) {
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
 * Makes this entity's sprites play their animation even when the game is suspended.
 */
void MapEntity::set_animation_ignore_suspend(bool ignore_suspend) {
  
  for (unsigned int i = 0; i < sprites.size(); i++) {
    Sprite *sprite = sprites[i];
    sprite->set_ignore_suspend(ignore_suspend);
  }
}

/**
 * Updates the entity.
 * This function is called repeteadly by the map. By default, it updates the position
 * of the entity according to its movement (if any), and it updates the sprites frames
 * if there are sprites.
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

