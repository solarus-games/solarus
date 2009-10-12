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
#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include "Common.h"
#include "entities/EntityType.h"
#include "entities/Layer.h"
#include "entities/EnemyAttack.h"
#include <vector>

/**
 * Abstract class for all objects placed on a map: tiles,
 * enemies, the hero, interactive objects, etc.
 * Each entity has:
 * - a layer on the map
 * - a position on the map (a rectangle),
 * - an origin point, relative to the rectangle's top-left corner
 */
class MapEntity {

 public:

   typedef MapEntity* (CreationFunction)(std::istream &is, Layer layer, int x, int y); /**< a function to parse a certain type of entity */
   static const CreationFunction* creation_functions[];                                /**< the creation functions of all types of entities */

  /**
   * Describes the features of each dynamic entity type:
   * is it an obstacle, can it detect collisions, etc.
   */
  struct EntityTypeFeatures {
    bool can_be_obstacle;         /**< Allows entities of this type to be obstacles for other entities.
				   * If enabled, the function is_obstacle_for() will be called
				   * to determine whether this is an obstacle or not. */
    bool can_detect_entities;     /**< Allows entities of this type to detect the presence 
				   * of the hero or other entities (this is possible only for
				   * suclasses of Detector). If enabled, the function 
				   * collision() will be called when a collision is detected. */
    bool can_be_displayed;        /**< Allows entities of this type to be displayed. 
				   * If enabled, the sprites added by the add_sprite() calls will be 
			 	   * displayed (if any). */
    bool is_displayed_in_y_order; /**< Allows an entity of this type to be displayed above
				   * the hero and other entities having this property when it is in front of them.
				   * This means that the displaying order of entities having this
				   * feature depends on their y position. The entities without this feature
				   * are displayed in the normal order (i.e. as specified by the map file), 
				   * and before the entities with the feature. */
  };
   
 private:

  /**
   * The features of each entity type.
   */
  static const EntityTypeFeatures entity_types_features[];

 protected:

  Map *map;                 /**< the map: this field is automatically set by class MapEntities after adding the entity to the map */

  // position (mandatory for all kinds of entities)

 private:

  Layer layer;              /**< Layer of the entity: LAYER_LOW, LAYER_INTERMEDIATE or LAYER_HIGH.
			     * The layer is constant for the tiles and can change for the hero and the dynamic entities.
			     * The layer is private to make sure the set_layer() function is always called.
			     */

 protected:
  SDL_Rect rectangle;       /**< This rectangle represents the position of the entity of the map and is
			     * used for the collision tests. It can be different from the sprite's
			     * rectangle of the entity.
			     * For example, the hero's position is a 16*16 rectangle, but its sprite may be
			     * a 24*32 rectangle.
			     */

  SDL_Rect origin;          /**< Coordinates of the origin point of the entity,
			     * relative to the top-left corner of its rectangle.
			     * Remember that when you call get_x() and get_y(), you get the coordinates
			     * of the origin point on the map, not the coordinates of the rectangle's
			     * top-left corner.
			     * This is useful because the top-left corner of the entity's rectangle does
			     * not represent the actual entity's coordinates.
			     */

  // other data, used for some kinds of entities

  std::string name;        /**< name of the entity, not used for all kinds of entities;
		            * the name identifies the entity in the game (an empty string
		            * indicates that the entity has no name) */

  int direction;           /**< direction of the entity, not used for all kinds of entities */

  std::vector<Sprite*> sprites; /**< sprite(s) representing the entity, not used for all kinds of entities because
				 * some of them are invisible, and some of them handle their sprites themselves */
  bool visible;            /**< indicates that this entity's sprites are currently displayed */
  Movement *movement;      /**< movement of the entity, not used for all kinds of entities;
			    * NULL indicates that the entity has no movement */

  // entity state

  bool suspended;          /**< indicates that the animation and movement of this entity are suspended */
  Uint32 when_suspended;   /**< indicates when this entity was suspended */
  bool being_removed;      /**< indicates that the entity is not valid anymore because it is about to be removed */

  // creation
  MapEntity(void);
  MapEntity(Layer layer, int x, int y, int width, int height);
  MapEntity(const std::string &name, int direction, Layer layer, int x, int y, int width, int height);

  // method called by the subclasses to set their properties
  void set_direction(int direction);
  void set_size(int width, int height);
  void set_size(SDL_Rect &size);
  void set_origin(int x, int y);
  void set_origin(const SDL_Rect &origin);
  void set_rectangle_from_sprite(void);
  void create_sprite(const SpriteAnimationSetId &id);
  void create_sprite(const SpriteAnimationSetId &id, bool enable_pixel_collisions);

  void set_movement(Movement *movement);
  void clear_movement(void);

 public:

  // destruction
  virtual ~MapEntity(void);
  void set_being_removed(void);
  bool is_being_removed(void);

  // entity type features
  virtual bool is_hero(void);
  virtual EntityType get_type(void) = 0;
  virtual bool can_be_obstacle(void);
  virtual bool can_detect_entities(void);
  virtual bool can_be_displayed(void);
  virtual bool is_displayed_in_y_order(void);

  // position in the map
  Layer get_layer(void);
  void set_layer(Layer layer);
  int get_x(void);
  int get_y(void);
  void set_x(int x);
  void set_y(int y);
  const SDL_Rect get_xy(void);
  void set_xy(const SDL_Rect &xy);
  void set_xy(int x, int y);

  int get_width(void);
  int get_height(void);
  const SDL_Rect & get_rectangle(void);
  void set_rectangle(const SDL_Rect &rectangle);
  const SDL_Rect & get_origin(void);
  int get_top_left_x(void);
  int get_top_left_y(void);
  void set_top_left_x(int x);
  void set_top_left_y(int y);
  void set_top_left_xy(int x, int y);

  virtual const SDL_Rect get_facing_point(void);
  virtual const SDL_Rect get_facing_point(int direction);
  const SDL_Rect get_center_point(void);

  bool is_aligned_to_grid(void);
  void set_aligned_to_grid(void);

  // properties
  virtual void set_map(Map *map);
  Map * get_map(void);
  const std::string& get_name(void) const;
  bool has_prefix(const std::string &prefix);
  int get_direction(void);

  // sprites
  Sprite * get_sprite(int index);
  Sprite * get_sprite(void);
  bool has_sprite(void);
  bool is_visible(void);
  void set_visible(bool visible);
  void set_animation_ignore_suspend(bool ignore_suspend);
  void start_fading(int direction);

  // movement
  Movement * get_movement(void);
  virtual void movement_just_changed(void);
  virtual void just_moved(void);
  virtual void set_facing_entity(Detector *detector);

  // collisions
  virtual bool is_obstacle_for(MapEntity *other);
  bool overlaps(const SDL_Rect &rectangle);
  bool overlaps(int x, int y);
  bool overlaps(MapEntity *other);
  bool is_origin_point_in(const SDL_Rect &rectangle);
  bool is_facing_point_in(const SDL_Rect &rectangle);
  bool is_center_in(const SDL_Rect &rectangle);

  double get_vector_angle(MapEntity *other);
  int get_distance(MapEntity *other);
  int get_distance(int x, int y);

  virtual void notify_collision_with_enemy(Enemy *enemy);
  virtual void notify_collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode);
  virtual void notify_collision_with_conveyor_belt(ConveyorBelt *conveyor_belt, int dx, int dy);
  virtual void notify_collision_with_sensor(Sensor *sensor);
  virtual void notify_collision_with_explosion(Explosion *explosion, Sprite *sprite_overlapping);
  virtual void notify_collision_with_enemy(Enemy *enemy, Sprite *enemy_sprite, Sprite *this_sprite);
  virtual void just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result);

  virtual bool is_water_obstacle(void);
  virtual bool is_hole_obstacle(void);
  virtual bool is_ladder_obstacle(void);
  virtual bool is_teletransporter_obstacle(Teletransporter *teletransporter);
  virtual bool is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt);
  virtual bool is_sensor_obstacle(Sensor *sensor);
  virtual bool is_raised_block_obstacle(CrystalSwitchBlock *raised_block);
  virtual bool is_crystal_switch_obstacle(CrystalSwitch *crystal_switch);
  virtual bool is_npc_obstacle(InteractiveEntity *npc);
  virtual bool is_enemy_obstacle(Enemy *enemy);
  virtual bool is_jump_sensor_obstacle(JumpSensor *jump_sensor);
  virtual bool is_destructible_item_obstacle(DestructibleItem *destructible_item);
  virtual bool is_sword_ignored(void);

  // suspended
  bool is_suspended(void);
  virtual void set_suspended(bool suspended);

  // update and display
  virtual void update(void);
  virtual void display_on_map(void);
};

#endif

