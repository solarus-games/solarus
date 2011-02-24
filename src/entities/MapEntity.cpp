/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
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
#include "entities/Stairs.h"
#include "entities/Arrow.h"
#include "entities/Hero.h"
#include "movements/Movement.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "Game.h"
#include "Map.h"

MapEntity::CreationFunction* MapEntity::creation_functions[] = {
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
  Stairs::parse,
};


const MapEntity::EntityTypeFeatures MapEntity::entity_types_features[] = {
  // can_be_obstacle, can_detect_entities, can_be_displayed, is_displayed_in_y_order
  // TODO isn't can_detect_entities stupid? just redefine the function in Detector should work
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
  { true,  true,  true, false}, // dynamic tile
  {false,  true,  true, false}, // switch
  { true, false, false, false}, // custom obstacle
  { true,  true, false, false}, // sensor
  { true,  true,  true, false}, // crystal switch
  { true,  true,  true, false}, // crystal switch block
  { true,  true,  true, false}, // shop item
  { true,  true,  true, false}, // conveyor belt
  { true,  true,  true, false}, // door
  { true,  true, false, false}, // stairs
  // other entity types (the ones not stored in map files) does not use this array and must redefine the 4 functions
};

const Rectangle MapEntity::directions_to_xy_moves[] = {
  Rectangle( 1, 0),
  Rectangle( 1,-1),
  Rectangle( 0,-1),
  Rectangle(-1,-1),
  Rectangle(-1, 0),
  Rectangle(-1, 1),
  Rectangle( 0, 1),
  Rectangle( 1, 1)
};

/**
 * @brief Creates a map entity without specifying its properties now.
 */
MapEntity::MapEntity():
  map(NULL),
  layer(LAYER_LOW),
  name(""),
  direction(0),
  first_sprite(NULL),
  visible(true),
  movement(NULL),
  old_movement(NULL),
  facing_entity(NULL),
  being_removed(false),
  enabled(true),
  waiting_enabled(false),
  suspended(false),
  when_suspended(0) {

  bounding_box.set_xy(0, 0);
  origin.set_xy(0, 0);
  set_size(0, 0);
}

/**
 * @brief Creates a map entity, specifying its position.
 *
 * The entity has no name and no direction.
 *
 * @param layer layer of the entity
 * @param x x position of the entity
 * @param y y position of the entity
 * @param width width of the entity
 * @param height height of the entity
 */
MapEntity::MapEntity(Layer layer, int x, int y, int width, int height):
  map(NULL),
  layer(layer),
  bounding_box(x, y),
  name(""),
  direction(0),
  first_sprite(NULL),
  visible(true),
  movement(NULL),
  old_movement(NULL),
  facing_entity(NULL),
  being_removed(false),
  enabled(true),
  waiting_enabled(false),
  suspended(false),
  when_suspended(0) {

  origin.set_xy(0, 0);
  set_size(width, height);
}

/**
 * @brief Creates an entity, specifying its position, its name and its direction.
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
  map(NULL),
  layer(layer),
  bounding_box(x, y),
  name(name),
  direction(direction),
  visible(true),
  movement(NULL),
  old_movement(NULL),
  facing_entity(NULL),
  being_removed(false),
  enabled(true),
  waiting_enabled(false),
  suspended(false),
  when_suspended(0) {

  origin.set_xy(0, 0);
  set_size(width, height);
}

/**
 * @brief Destructor.
 *
 * The sprite and the movement of the entity are deleted (if any).
 */
MapEntity::~MapEntity() {

  remove_sprites();
  clear_movement();
  if (old_movement != NULL) {
    delete old_movement;
  }
}

/**
 * @brief Returns whether this entity is the hero controlled by the player.
 * @return true if this entity is the hero
 */
bool MapEntity::is_hero() {
  return get_type() == HERO;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * @return true if this type of entity can be obstacle for other entities
 */
bool MapEntity::can_be_obstacle() {
  return entity_types_features[get_type()].can_be_obstacle;
}

/**
 * @brief Returns whether entities of this type have detection capabilities.
 *
 * This function returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 *
 * @return true if this type of entity can detect other entities
 */
bool MapEntity::can_detect_entities() {
  return entity_types_features[get_type()].can_detect_entities;
}

/**
 * @brief Returns whether entities of this type can be displayed.
 *
 * If yes, the sprites added by the add_sprite() calls will be
 * displayed (if any).
 *
 * @return true if this type of entity can be displayed
 */
bool MapEntity::can_be_displayed() {
  return entity_types_features[get_type()].can_be_displayed;
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file),
 * and before the entities with the feature.
 *
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool MapEntity::is_displayed_in_y_order() {
  return entity_types_features[get_type()].is_displayed_in_y_order;
}

/**
 * @brief Returns whether this entity is on a map, that is,
 * whether set_map() has been called.
 * @return true if the entity is on a map
 */
bool MapEntity::is_on_map() {
  return map != NULL;
}

/**
 * @brief Sets the map where this entity is.
 *
 * Warning: when this function is called during the initialization of a new map,
 * the current map of the game is still the old one.
 *
 * @param map the map
 */
void MapEntity::set_map(Map &map) {

  this->map = &map;
}

/**
 * @brief Notifies this entity that its map has just become active.
 */
void MapEntity::notify_map_started() {

  // notify the sprites (useful for tileset-dependent sprites such as doors and blocks)
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {

    Sprite &sprite = *(it->second);
    sprite.set_map(*map);
  }
}

/**
 * @brief Returns the map where this entity is.
 * @return the map
 */
Map& MapEntity::get_map() {
  return *map;
}

/**
 * @brief Returns the game that is running the map where this entity is.
 * @return the game
 */
Game& MapEntity::get_game() {
  return map->get_game();
}

/**
 * @brief Returns the entities of the current map.
 * @return the entities
 */
MapEntities& MapEntity::get_entities() {
  return map->get_entities();
}

/**
 * @brief Returns the script of the current map.
 * @return the map script
 */
MapScript& MapEntity::get_map_script() {
  return map->get_script();
}

/**
 * @brief Returns the current equipment.
 * @return the equipment
 */
Equipment& MapEntity::get_equipment() {
  return get_game().get_equipment();
}

/**
 * @brief Returns the keys effect manager.
 * @return the keys effect
 */
KeysEffect& MapEntity::get_keys_effect() {
  return get_game().get_keys_effect();
}

/**
 * @brief Returns the game controls.
 * @return the controls
 */
GameControls& MapEntity::get_controls() {
  return get_game().get_controls();
}

/**
 * @brief Returns the dialog box manager.
 * @return the dialog box
 */
DialogBox& MapEntity::get_dialog_box() {
  return get_game().get_dialog_box();
}

/**
 * @brief Returns the savegame.
 * @return the savegame
 */
Savegame& MapEntity::get_savegame() {
  return get_game().get_savegame();
}


/**
 * @brief Returns the hero
 * @return the hero
 */
Hero& MapEntity::get_hero() {
  return get_entities().get_hero();
}

/**
 * @brief Schedules this entity for removal.
 *
 * The entity will be removed from the map and destroyed.
 * This function is equivalent to calling map->get_entities()->remove_entity(this).
 */
void MapEntity::remove_from_map() {

  get_map().get_entities().remove_entity(this);
}

/**
 * @brief Notifies this entity that it is about to be removed.
 *
 * This function is called when the entity has just been added
 * to the list of entities that will be removed from the map
 * and deleted from the memory as soon as possible.
 */
void MapEntity::notify_being_removed() {

  this->being_removed = true;
}

/**
 * @brief Returns true if this entity is about to be deleted.
 *
 * When this function returns true, the entity is not
 * considered to be on the map anymore.
 *
 * @return true if this entity is about to be deleted
 */
bool MapEntity::is_being_removed() {
  return being_removed;
}

/**
 * @brief Returns the layer of the entity on the map.
 * @return the layer of the entity on the map
 */
Layer MapEntity::get_layer() {
  return layer;
}

/**
 * @brief Sets the layer of the entity on the map.
 *
 * If the entity is stored on the map in the class MapEntities,
 * you should not call this function directly: call
 * MapEntities::set_entity_layer() instead because the class MapEntities
 * stores different lists of entities for each layer.
 * TODO: use notify_layer_changed to update MapEntities instead
 *
 * @param layer the layer of the entity on the map
 */
void MapEntity::set_layer(Layer layer) {

  this->layer = layer;
  notify_layer_changed();
}

/**
 * @brief This function is called when the layer of this entity has just changed.
 *
 * Redefine it if you need to be notified.
 */
void MapEntity::notify_layer_changed() {
  // nothing done by default
}

/**
 * @brief Returns the direction of the entity.
 *
 * This direction is not used by all kinds of entities
 * since some of them only use the direction property of their sprites
 * and/or their movements.
 *
 * @return the direction of the entity
 */
int MapEntity::get_direction() {
  return direction;
}

/**
 * @brief Sets the direction property of this entity.
 * @param direction the direction
 */
void MapEntity::set_direction(int direction) {
  this->direction = direction;
}

/**
 * @brief Returns the current x position of the entity.
 * @return the x position of the entity
 */
int MapEntity::get_x() {
  return bounding_box.get_x() + origin.get_x();
}

/**
 * @brief Returns the current y position of the entity.
 * @return the y position of the entity
 */
int MapEntity::get_y() {
  return bounding_box.get_y() + origin.get_y();
}

/**
 * @brief Sets the x position of the entity.
 *
 * This function is called by the movement object of this entity.
 *
 * @param x the new x position
 */
void MapEntity::set_x(int x) {
  bounding_box.set_x(x - origin.get_x());
}

/**
 * @brief Sets the y position of the entity.
 *
 * This function is called by the movement object of this entity.
 *
 * @param y the new y position
 */
void MapEntity::set_y(int y) {
  bounding_box.set_y(y - origin.get_y());
}

/**
 * @brief Returns the coordinates of the origin point of the entity, relative to the map.
 *
 * These are the coordinates of the point as returned by get_x() and get_y().
 *
 * @return the coordinates of the entity on the map
 */
const Rectangle MapEntity::get_xy() {
  return Rectangle(get_x(), get_y());
}

/**
 * @brief Sets the coordinates of the origin point of the entity, relative to the map.
 *
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 *
 * @param x the new x coordinate of the entity on the map
 * @param y the new y coordinate of the entity on the map
 */
void MapEntity::set_xy(int x, int y) {
  set_x(x);
  set_y(y);
}

/**
 * @brief Sets the coordinates of the origin point of the entity, relative to the map.
 * 
 * This function sets the coordinates of the point as returned by get_x() and get_y().
 *
 * @param xy the new coordinates of the entity on the map (the width and height are ignored)
 */
void MapEntity::set_xy(const Rectangle &xy) {
  set_xy(xy.get_x(), xy.get_y());
}

/**
 * @brief Returns the x position of the entity's top-left corner.
 * @return the x position of the entity's top-left corner
 */
int MapEntity::get_top_left_x() {
  return bounding_box.get_x();
}

/**
 * @brief Returns the y position of the entity's top-left corner.
 * @return the y position of the entity's top-left corner
 */
int MapEntity::get_top_left_y() {
  return bounding_box.get_y();
}

/**
 * @brief Sets the x position of the entity's top-left corner.
 * @param x the new top-left x position
 */
void MapEntity::set_top_left_x(int x) {
  bounding_box.set_x(x);
}

/**
 * @brief Sets the y position of the entity's top-left corner.
 * @param y the new top-left y position
 */
void MapEntity::set_top_left_y(int y) {
  bounding_box.set_y(y);
}

/**
 * @brief Sets the position of the entity.
 *
 * This function sets the coordinates of the rectangle's top-left corner.
 *
 * @param x x position of the entity
 * @param y y position of the entity
 */
void MapEntity::set_top_left_xy(int x, int y) {
  set_top_left_x(x);
  set_top_left_y(y);
}

/**
 * @brief Returns the coordinates of the origin point of the entity, relative to the map.
 *
 * These are the coordinates of the point as returned by get_x() and get_y().
 *
 * @return the coordinates of the entity on the map
 */
const Rectangle MapEntity::get_displayed_xy() {

  if (get_movement() == NULL) {
    return get_xy();
  }

  return get_movement()->get_displayed_xy();
}

/**
 * @brief Returns the width of the entity.
 * @return the width of the entity
 */
int MapEntity::get_width() {
  return bounding_box.get_width();
}

/**
 * @brief Returns the height of the entity.
 * @return the height of the entity
 */
int MapEntity::get_height() {
  return bounding_box.get_height();
}

/**
 * @brief Returns the size of the entity.
 * @return a rectangle whose width and height represent the size of the entity
 * (its coordinates should be ignored)
 */
const Rectangle& MapEntity::get_size() {
  return bounding_box;
}

/**
 * @brief Sets the size of the entity.
 * @param width the entity's width
 * @param height the entity's height
 */
void MapEntity::set_size(int width, int height) {
  bounding_box.set_size(width, height);
}

/**
 * @brief Sets the size of the entity.
 * @param size a rectangle having the width and height to set to the entity
 */
void MapEntity::set_size(const Rectangle &size) {
  bounding_box.set_size(size);
}

/**
 * @brief Returns the bounding box of the entity.
 *
 * This function returns the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 *
 * @return the position and size of the entity
 */
const Rectangle& MapEntity::get_bounding_box() {
  return bounding_box;
}

/**
 * @brief Sets the bounding box of the entity.
 *
 * This function sets the rectangle defined by
 * get_top_left_x(), get_top_left_y(), get_width() and get_height().
 *
 * @param bounding_box the new position and size of the entity
 */
void MapEntity::set_bounding_box(const Rectangle &bounding_box) {
  this->bounding_box = bounding_box;
}

/**
 * @brief Sets the origin point and the size of the entity like its sprite.
 *
 * You may call this function only if the entity's bounding box
 * is the same as the sprite's rectangle.
 * Otherwise, you have to call set_size() and set_origin()
 * explicitely.
 */
void MapEntity::set_bounding_box_from_sprite() {

  Sprite &sprite = get_sprite();
  set_size(sprite.get_size());
  set_origin(sprite.get_origin());
}

/**
 * @brief Returns whether the entity's bounding box is aligned with the 8*8 grid of the map.
 * @return true if the entity's bounding box is aligned
 */
bool MapEntity::is_aligned_to_grid() {
  return is_aligned_to_grid_x() && is_aligned_to_grid_y();
}

/**
 * @brief Returns whether the entity's top-left corner is aligned
 * horizontally with the 8*8 grid of the map.
 * @return true if the entity's top-left corner is aligned hotizontally
 */
bool MapEntity::is_aligned_to_grid_x() {
  return get_top_left_x() % 8 == 0;
}

/**
 * @brief Returns whether the entity's top-left corner is aligned
 * vertically with the 8*8 grid of the map.
 * @return true if the entity's top-left corner is aligned vertically
 */
bool MapEntity::is_aligned_to_grid_y() {
  return get_top_left_y() % 8 == 0;
}

/**
 * @brief Makes the entity's top-left corner aligned with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void MapEntity::set_aligned_to_grid() {

  set_aligned_to_grid_x();
  set_aligned_to_grid_y();
}

/**
 * @brief Makes the entity's top-left corner aligned horizontally
 * with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void MapEntity::set_aligned_to_grid_x() {

  int x = get_top_left_x() + 4;
  x -= x % 8;
  set_top_left_x(x);
}

/**
 * @brief Makes the entity's top-left corner aligned vertically
 * with the 8*8 grid of the map.
 *
 * Be careful: This function does not check the collisions with obstacles.
 */
void MapEntity::set_aligned_to_grid_y() {

  int y = get_top_left_y() + 4;
  y -= y % 8;
  set_top_left_y(y);
}

/**
 * @brief Returns the coordinates of the point the entity is looking at.
 *
 * You should redefine this method to define what is a facing point for your subclass.
 *
 * @return the coordinates of the point the entity is looking at
 */
const Rectangle MapEntity::get_facing_point() {
  return Rectangle(-1, -1);
}

/**
 * @brief Returns the coordinates of the point the entity would be facing
 * if it was looking towards the specified direction.
 *
 * You should redefine this method to define what is a facing point for your subclass.
 *
 * @param direction a direction (0 to 3)
 * @return the coordinates of the point the entity in this direction
 */
const Rectangle MapEntity::get_facing_point(int direction) {
  return Rectangle(-1, -1);
}

/**
 * @brief Returns the detector in front of this entity.
 * @return the detector this entity is touching, or NULL if there is no detector in front of him
 */
Detector* MapEntity::get_facing_entity() {
  return facing_entity;
}

/**
 * @brief Sets the entity this entity is currently facing.
 *
 * This function is called when this entity is facing a new detector.
 *
 * @param facing_entity the detector this entity is now facing (possibly NULL)
 */
void MapEntity::set_facing_entity(Detector* facing_entity) {
  this->facing_entity = facing_entity;
  notify_facing_entity_changed(facing_entity);
}

/**
 * @brief Notifies this entity that its facing entity has just changed.
 * @param facing_entity the detector this entity is now facing (possibly NULL)
 */
void MapEntity::notify_facing_entity_changed(Detector* facing_entity) {
}

/**
 * @brief Returns the coordinates of the center point of the entity's rectangle.
 * @return the coordinates of the center point of the entity
 */
const Rectangle MapEntity::get_center_point() {
  return bounding_box.get_center();
}

/**
 * @brief Returns the name of the entity (if any).
 * @return the name of the entity, or an empty string if the entity is not identifiable
 */
const std::string& MapEntity::get_name() const {
  return name;
}

/**
 * @brief Returns whether the name of this entity starts with the specified prefix.
 * @param prefix a prefix
 * @return true if the name starts with this prefix
 */
bool MapEntity::has_prefix(const std::string &prefix) {
  return name.substr(0, prefix.size()) == prefix;
}

/**
 * @brief Returns the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * @return the origin point
 */
const Rectangle& MapEntity::get_origin() {
  return origin;
}

/**
 * @brief Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * @param x x coordinate of the origin
 * @param y y coordinate of the origin
 */
void MapEntity::set_origin(int x, int y) {

  bounding_box.add_xy(origin.get_x() - x, origin.get_y() - y);
  origin.set_xy(x, y);
}

/**
 * @brief Sets the origin point of the entity,
 * relative to the top-left corner of its rectangle.
 * @param origin x and y coordinates of the origin
 */
void MapEntity::set_origin(const Rectangle &origin) {
  set_origin(origin.get_x(), origin.get_y());
}

/**
 * @brief Returns the number of sprites of this entity.
 * @return the number of sprites created
 */
int MapEntity::get_nb_sprites() {
  return sprites.size();
}

/**
 * @brief Returns whether the entity has at least one sprite.
 * @return true if the entity has at least one sprite.
 */
bool MapEntity::has_sprite() {
  return sprites.size() != 0;
}

/**
 * @brief Returns whether the entity has a sprite with the specified animation set name.
 * @param id name of an animation set
 * @return true if the entity has a sprite with this animation set.
 */
bool MapEntity::has_sprite(const SpriteAnimationSetId &id) {
  return sprites.count(id) > 0;
}

/**
 * @brief Returns the sprite created with the first call to create_sprite() for this entity.
 * @return the first sprite created
 */
Sprite& MapEntity::get_sprite() {
  return *first_sprite;
}

/**
 * @brief Returns a sprite of the entity, previously created with a call to create_sprite().
 * @param id name of the animation set of the sprite to get
 * @return the sprite with the specified animation set
 */
Sprite& MapEntity::get_sprite(const SpriteAnimationSetId &id) {

  Debug::check_assertion(has_sprite(id),
    StringConcat() << "Cannot find sprite '" << id << "' for entity '" << get_name() << "'");

  return *sprites[id];
}

/**
 * @brief Returns all sprites of this entity.
 * @return the sprites indexed by their animation set id
 */
std::map<SpriteAnimationSetId, Sprite*>& MapEntity::get_sprites() {
  return sprites;
}

/**
 * @brief Adds a sprite to this entity.
 * @param id id of the sprite's animations to add
 * @param enable_pixel_collisions true to enable the pixel-perfect collision tests for this sprite
 */
void MapEntity::create_sprite(const SpriteAnimationSetId &id, bool enable_pixel_collisions) {

  Sprite* sprite = new Sprite(id);

  if (enable_pixel_collisions) {
    sprite->enable_pixel_collisions();
  }

  if (!has_sprite()) {
    // first sprite created
    first_sprite = sprite;
  }

  sprites[id] = sprite;
}

/**
 * @brief Removes the specified sprite from this entity.
 *
 * The sprite is destroyed.
 *
 * @param id id of the sprite's animation set
 */
void MapEntity::remove_sprite(const SpriteAnimationSetId &id) {

  Sprite *sprite = &get_sprite(id);

  if (sprite == first_sprite) {
    first_sprite = NULL;
  }
  delete sprite;
  sprites.erase(id);
}

/**
 * @brief Removes and destroys all sprites of this entity.
 */
void MapEntity::remove_sprites() {
  
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {
    delete it->second;
  }
  sprites.clear();
}

/**
 * @brief Notifies this entity that the frame of one of its sprites has just changed.
 *
 * By default, nothing is done.
 *
 * @param sprite the sprite
 * @param animation the current animation
 * @param frame the new frame
 */
void MapEntity::notify_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame) {
}

/**
 * @brief Notifies this entity that the animation of one of its sprites
 * has just finished.
 *
 * By default, nothing is done.
 *
 * @param sprite the sprite
 * @param animation the animation just finished
 */
void MapEntity::notify_sprite_animation_finished(Sprite& sprite, const std::string& animation) {
}

/**
 * @brief Returns whether this entity is currently visible.
 * @return true if this entity is currently visible
 */
bool MapEntity::is_visible() {
  return visible;
}

/**
 * @brief Sets whether this entity is visible.
 * @param visible true to make it visible
 */
void MapEntity::set_visible(bool visible) {
  this->visible = visible;
}

/**
 * @brief Returns the current movement of the entity.
 * @return the entity's movement, or NULL if there is no movement
 */
Movement* MapEntity::get_movement() {
  return movement;
}

/**
 * @brief Sets the movement of this entity.
 *
 * Once you have called this function, the pointer to the movement is managed by the entity only.
 * Never delete it from outside! The movement will be deleted if clear_movement() is called
 * or when the entity is destroyed.
 *
 * If a previous movement was already set, it is not deleted (so that you can reassign it later).
 * Thus, most of the time, you should call clear_movement() before set_movement() to avoid a memory leak.
 *
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
 * @brief Destroys the movement of this entity.
 *
 * The entity immediately stops moving.
 * The movement object will be destroyed at the next cycle,
 * thus this function can be called by the movement object itself.
 */
void MapEntity::clear_movement() {

  if (old_movement != NULL) {
    delete old_movement;
  }

  old_movement = movement; // destroy it later
  movement = NULL;
}

/**
 * @brief Notifies this entity that it has just tried to change its position.
 *
 * This function is called only when the movement is not suspended.
 * By default, nothing is done.
 *
 * @param success true if the position has actually just changed
 */
void MapEntity::notify_movement_tried(bool success) {
}

/**
 * @brief This function is called when the entity has just moved.
 *
 * It checks collisions with the detectors on the map.
 */
void MapEntity::notify_position_changed() {

  check_collision_with_detectors();
}

/**
 * @brief Checks collisions between this entity and the detectors of the map.
 */
void MapEntity::check_collision_with_detectors() {

  // detect simple collisions
  get_map().check_collision_with_detectors(*this);

  // detect pixel-precise collisions
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {

    Sprite &sprite = *(it->second);
    if (sprite.are_pixel_collisions_enabled()) {
      get_map().check_collision_with_detectors(*this, sprite);
    }
  }
}

/**
 * @brief This function can be called by the movement object
 * to notify the entity when the movement has just changed
 * (e.g. the speed, the angle or the trajectory).
 *
 * By default, nothing is done.
 * TODO: actually call this function from all movement subclasses
 */
void MapEntity::notify_movement_changed() {
}

/**
 * @brief This function is called when the movement of the entity is finished.
 *
 * By default, nothing is done.
 */
void MapEntity::notify_movement_finished() {
}

/**
 * @brief Converts a direction into the corresponding one-pixel move on x and y.
 * @param direction8 a direction (0 to 7)
 * @return a rectangle with x and y set to -1, 0 or 1 depending on the direction
 */
const Rectangle& MapEntity::direction_to_xy_move(int direction8) {
  return directions_to_xy_moves[direction8];
}

/**
 * @brief Returns whether this entity is enabled.
 * @return true if this entity is enabled
 */
bool MapEntity::is_enabled() {
  return enabled;
}

/**
 * @brief Enables or disables this entity.
 * @param enabled true to enable the entity, false to disable it
 */
void MapEntity::set_enabled(bool enabled) {

  if (enabled) {
    // enable the entity as soon as possible
    this->waiting_enabled = true;
  }
  else {
    this->enabled = false;
    this->waiting_enabled = false;
    notify_enabled(false);
  }
}

/**
 * @brief Notifies this entity that it was just enabled or disabled.
 * @param enabled true if the entity is now enabled
 */
void MapEntity::notify_enabled(bool enabled) {
}

/**
 * @brief Returns whether this entity is an obstacle for another one when
 * it is enabled.
 *
 * By default, this function returns false.
 *
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool MapEntity::is_obstacle_for(MapEntity &other) {
  return false;
}

/**
 * @brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 *
 * This function returns false by default.
 *
 * @return true if this entity can collide with entities that are on another layer
 */
bool MapEntity::has_layer_independent_collisions() {
  return false;
}

/**
 * @brief Returns whether a shallow water tile is currently considered as an obstacle by this entity.
 *
 * This function returns is_deep_water_obstacle() by default.
 *
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool MapEntity::is_shallow_water_obstacle() {
  return is_deep_water_obstacle();
}

/**
 * @brief Returns whether a deep water tile is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @return true if the deep water tiles are currently an obstacle for this entity
 */
bool MapEntity::is_deep_water_obstacle() {
  return true;
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @return true if the holes are currently an obstacle for this entity
 */
bool MapEntity::is_hole_obstacle() {
  return true;
}

/**
 * @brief Returns whether a ladder is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @return true if the ladders are currently an obstacle for this entity
 */
bool MapEntity::is_ladder_obstacle() {
  return true;
}

/**
 * @brief Returns whether the hero is currently considered as an obstacle by this entity.
 *
 * This function returns false by default.
 *
 * @param hero the hero
 * @return true if the hero is currently an obstacle for this entity
 */
bool MapEntity::is_hero_obstacle(Hero &hero) {
  return false;
}

/**
 * @brief Returns whether a block is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @param block a block
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool MapEntity::is_block_obstacle(Block &block) {
  return true;
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool MapEntity::is_teletransporter_obstacle(Teletransporter &teletransporter) {
  return true;
}

/**
 * @brief Returns whether a conveyor belt is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool MapEntity::is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt) {
  return true;
}

/**
 * @brief Returns whether some stairs are currently considered as an obstacle for this entity.
 *
 * This function returns true by default.
 *
 * @param stairs an stairs entity
 * @return true if the stairs are currently an obstacle for this entity
 */
bool MapEntity::is_stairs_obstacle(Stairs &stairs) {
  return true;
}

/**
 * @brief Returns whether a sensor is currently considered as an obstacle by this entity.
 *
 * This function returns false by default.
 *
 * @param sensor a sensor
 * @return true if the sensor are currently an obstacle for this entity
 */
bool MapEntity::is_sensor_obstacle(Sensor &sensor) {
  return false;
}

/**
 * @brief Returns whether a raised crystal switch block is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @param raised_block a crystal switch block raised
 * @return true if the raised block is currently an obstacle for this entity
 */
bool MapEntity::is_raised_block_obstacle(CrystalSwitchBlock &raised_block) {
  return true;
}

/**
 * @brief Returns whether a crystal switch is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 * It should be redefined only for entities that can activate a crystal switch: a pot, the boomerang, etc.
 *
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool MapEntity::is_crystal_switch_obstacle(CrystalSwitch &crystal_switch) {
  return true;
}

/**
 * @brief Returns whether a non-playing character is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @param npc a non-playing character
 * @return true if the NPC is currently an obstacle for this entity
 */
bool MapEntity::is_npc_obstacle(InteractiveEntity &npc) {
  return true;
}

/**
 * @brief Returns whether an enemy is currently considered as an obstacle by this entity.
 *
 * This function returns false by default.
 *
 * @param enemy an enemy
 * @return true if the enemy is currently an obstacle for this entity
 */
bool MapEntity::is_enemy_obstacle(Enemy &enemy) {
  return false;
}

/**
 * @brief Returns whether a non-diagonal jump sensor is currently considered as an obstacle by this entity.
 *
 * This function returns true by default.
 *
 * @param jump_sensor a non-diagonal jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool MapEntity::is_jump_sensor_obstacle(JumpSensor &jump_sensor) {
  return true;
}

/**
 * @brief Returns whether a destructible item is currently considered as an obstacle by this entity.
 *
 * By default, this function returns true unless the destructible item is disabled
 * (e.g. a bomb flower that will regenerate).
 *
 * @param destructible_item a destructible item
 * @return true if the destructible item is currently an obstacle for this entity
 */
bool MapEntity::is_destructible_item_obstacle(DestructibleItem &destructible_item) {

  return !destructible_item.is_disabled();
}

/**
 * @brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero taps this entity with the sword.
 *
 * @return true if the sword is ignored
 */
bool MapEntity::is_sword_ignored() {
  return false;
}

/**
 * @brief Returns whether or not this entity's bounding box overlaps a specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's bounding box overlaps the specified rectangle
 */
bool MapEntity::overlaps(const Rectangle &rectangle) {
  return bounding_box.overlaps(rectangle);
}

/**
 * @brief Returns whether or not a point overlaps this entity's bounding box.
 * @param x x coordinate of the point to check
 * @param y y coordinate of the point to check
 * @return true if the point is in this entity's bounding box
 */
bool MapEntity::overlaps(int x, int y) {
  return bounding_box.contains(x, y);
}

/**
 * @brief Returns whether or not this entity's bounding box overlaps
 * another entity's bounding box.
 * @param other another entity
 * @return true if this entity's bounding box overlaps the other entity's bounding box
 */
bool MapEntity::overlaps(MapEntity &other) {
  return overlaps(other.get_bounding_box());
}

/**
 * @brief Returns whether or not this entity's origin point is in
 * the specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's origin point is in the specified rectangle
 */
bool MapEntity::is_origin_point_in(const Rectangle &rectangle) {
  return rectangle.contains(get_x(), get_y()); 
}

/**
 * @brief Returns whether this entity's facing point is in
 * the specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's facing point is in the specified rectangle 
 */
bool MapEntity::is_facing_point_in(const Rectangle &rectangle) {

  const Rectangle &facing_point = get_facing_point();
  return rectangle.contains(facing_point.get_x(), facing_point.get_y());
}

/**
 * @brief Returns whether a facing point of this entity is in
 * the specified rectangle.
 * @param rectangle the rectangle to check
 * @param direction direction of the facing point to consider (0 to 3)
 * @return true if this facing point is in the specified rectangle
 */
bool MapEntity::is_facing_point_in(const Rectangle &rectangle, int direction) {

  const Rectangle &facing_point = get_facing_point(direction);
  return rectangle.contains(facing_point.get_x(), facing_point.get_y());
}

/**
 * @brief Returns whether or not this entity's center is in
 * the specified rectangle.
 * @param rectangle the rectangle to check
 * @return true if this entity's center is in the specified rectangle
 */
bool MapEntity::is_center_in(const Rectangle &rectangle) {

  const Rectangle &center = get_center_point();
  return rectangle.contains(center.get_x(), center.get_y());
}

/**
 * @brief Returns the angle of the vector between the origin of this entity
 * and the origin of another entity.
 * @param other the other entity
 * @return the angle of the vector in radians
 */
double MapEntity::get_vector_angle(MapEntity &other) {
  return Geometry::get_angle(get_x(), get_y(), other.get_x(), other.get_y());
}

/**
 * @brief Returns the distance between the origin of this entity and a point.
 * @param x x coordinate of the point
 * @param y y coordinate of the point
 * @return the distance between this entity and the point in pixels
 */
int MapEntity::get_distance(int x, int y) {
  return (int) Geometry::get_distance(get_x(), get_y(), x, y);
}

/**
 * @brief Returns the distance between the origin of this entity
 * and the origin of another entity.
 * @param other the other entity
 * @return the distance between the two entities in pixels
 */
int MapEntity::get_distance(MapEntity &other) {
  return (int) Geometry::get_distance(get_x(), get_y(), other.get_x(), other.get_y());
}

/**
 * @brief This function is called when a destructible item detects a non-pixel perfect collision with this entity.
 * @param destructible_item the destructible item
 * @param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_destructible_item(DestructibleItem &destructible_item, CollisionMode collision_mode) {
}

/**
 * @brief This function is called when a teletransporter detects a collision with this entity.
 * @param teletransporter the teletransporter
 * @param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_teletransporter(Teletransporter &teletransporter, CollisionMode collision_mode) {
}

/**
 * @brief This function is called when a conveyor belt detects a collision with this entity.
 * @param conveyor_belt a conveyor belt
 * @param dx direction of the x move in pixels (0, 1 or -1)
 * @param dy direction of the y move in pixels (0, 1 or -1)
 */
void MapEntity::notify_collision_with_conveyor_belt(ConveyorBelt &conveyor_belt, int dx, int dy) {
}

/**
 * @brief This function is called when a stairs entity detects a collision with this entity.
 * @param stairs the stairs
 * @param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_stairs(Stairs &stairs, CollisionMode collision_mode) {
}

/**
 * @brief This function is called when a jump sensor detects a collision with this entity.
 * @param jump_sensor the jump sensor
 */
void MapEntity::notify_collision_with_jump_sensor(JumpSensor &jump_sensor) {
}

/**
 * @brief This function is called when a sensor detects a collision with this entity.
 * @param sensor a sensor
 */
void MapEntity::notify_collision_with_sensor(Sensor &sensor) {
}

/**
 * @brief This function is called when a switch detects a collision with this entity.
 * @param sw a switch
 */
void MapEntity::notify_collision_with_switch(Switch &sw) {
}

/**
 * @brief This function is called when a crystal switch detects a collision with this entity.
 * @param crystal_switch the crystal switch
 * @param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_crystal_switch(CrystalSwitch &crystal_switch, CollisionMode collision_mode) {
}

/**
 * @brief This function is called when a the sprite of a crystal switch 
 * detects a pixel-perfect collision with a sprite of this entity.
 * @param crystal_switch the crystal switch
 * @param sprite_overlapping the sprite of the current entity that collides with the crystal switch
 */
void MapEntity::notify_collision_with_crystal_switch(CrystalSwitch &crystal_switch, Sprite &sprite_overlapping) {
}

/**
 * @brief This function is called when bomb detects a collision with a this entity.
 * @param bomb the bomb
 * @param collision_mode the collision mode that detected the event
 */
void MapEntity::notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode) {
}

/**
 * @brief This function is called when an explosion's sprite
 * detects a pixel-perfect collision with a sprite of this entity.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of the current entity that collides with the explosion
 */
void MapEntity::notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping) {
}

/**
 * @brief This function is called when an enemy's rectangle detects a collision with this entity's rectangle.
 * @param enemy the enemy
 */
void MapEntity::notify_collision_with_enemy(Enemy &enemy) {
}

/**
 * @brief This function is called when an enemy's sprite collides with a sprite of this entity
 * @param enemy the enemy
 * @param enemy_sprite the enemy's sprite that overlaps a sprite of this entity
 * @param this_sprite this entity's sprite that overlaps the enemy's sprite
 */
void MapEntity::notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite) {
}

/**
 * @brief Notifies this entity that it has just attacked an enemy.
 *
 * This function is called even if this attack was not successful.
 *
 * @param attack the attack
 * @param victim the enemy just attacked
 * @param result indicates how the enemy reacted to the attack
 * @param killed indicates that the attack has just killed the enemy
 */
void MapEntity::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {
}

/**
 * @brief Returns whether the movement and the animations of this entity are suspended.
 * @return true if the movement and the animations are suspended
 */
bool MapEntity::is_suspended() {
  return suspended;
}

/**
 * @brief Suspends or resumes the movement and the animations of this entity.
 *
 * This function is called by the map when the game is suspended or resumed.
 *
 * @param suspended true to suspend the movement and the animations, false to resume them
 */
void MapEntity::set_suspended(bool suspended) {

  this->suspended = suspended;

  // remember the date if the entity is being suspended
  if (suspended) {
    when_suspended = System::now();
  }

  // suspend/unsuspend the sprites animations
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {
    
    Sprite &sprite = *(it->second);
    sprite.set_suspended(suspended);
  }

  // suspend/unsuspend the movement
  if (movement != NULL) {
    movement->set_suspended(suspended);
  }
}

/**
 * @brief Makes this entity's sprites play their animation even when the game is suspended.
 * @param ignore_suspend true to keep playing the sprites when the game is suspended
 */
void MapEntity::set_animation_ignore_suspend(bool ignore_suspend) {
  
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {
    
    Sprite &sprite = *(it->second);
    sprite.set_ignore_suspend(ignore_suspend);
  }
}

/**
 * @brief Displays a fade-in or fade-out effect on the entity's sprites.
 * @param direction fade direction (0: in, 1: out)
 */
void MapEntity::start_fading(int direction) {

  // update the sprites
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {
    
    Sprite &sprite = *(it->second);
    sprite.start_fading(direction);
  }
}

/**
 * @brief Updates the entity.
 *
 * This function is called repeteadly by the map. By default, it updates the position
 * of the entity according to its movement (if any), and it updates the sprites frames
 * if there are sprites.
 * Redefine it in subclasses for the entities that should be updated
 * for other treatments but don't forget to call this method
 * to handle the movement and the sprites.
 */
void MapEntity::update() {

  // enable if necessary
  if (waiting_enabled) {
    Hero &hero = get_hero();
    if (!is_obstacle_for(hero) || !overlaps(hero)) {
      this->enabled = true;
      this->waiting_enabled = false;
      notify_enabled(true);
    }
  }

  // check the facing entity
  if (facing_entity != NULL && facing_entity->is_being_removed()) {
    set_facing_entity(NULL);
  }

  // update the sprites
  std::map<std::string, Sprite*>::iterator it;
  for (it = sprites.begin(); it != sprites.end(); it++) {

    Sprite &sprite = *(it->second);
    sprite.update();
    if (sprite.has_frame_changed()) {

      if (sprite.are_pixel_collisions_enabled()) {
        get_map().check_collision_with_detectors(*this, sprite);
      }

      notify_sprite_frame_changed(sprite, sprite.get_current_animation(), sprite.get_current_frame());
      if (sprite.is_animation_finished()) {
        notify_sprite_animation_finished(sprite, sprite.get_current_animation());
      }
    }
  }

  // update the movement
  if (movement != NULL) {
    movement->update();
  }

  if (old_movement != NULL) {
    delete old_movement;
    old_movement = NULL;
  }
}

/**
 * @brief Displays the entity on the map.
 *
 * By default, this function displays the entity's sprites (if any).
 */
void MapEntity::display_on_map() {

  if (is_visible()) {
    // display the sprites
    std::map<std::string, Sprite*>::iterator it;
    for (it = sprites.begin(); it != sprites.end(); it++) {

      Sprite &sprite = *(it->second);
      get_map().display_sprite(sprite, get_displayed_xy());
    }
  }
}

