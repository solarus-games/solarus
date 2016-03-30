/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_ENTITY_H
#define SOLARUS_ENTITY_H

#include "solarus/Common.h"
#include "solarus/lua/ExportableToLua.h"
#include "solarus/entities/EntityType.h"
#include "solarus/entities/Ground.h"
#include "solarus/entities/CollisionMode.h"
#include "solarus/entities/EnemyAttack.h"
#include "solarus/entities/EnemyReaction.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/GameCommand.h"
#include "solarus/SpritePtr.h"
#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>

struct lua_State;

namespace Solarus {

class Block;
class Bomb;
class Chest;
class CommandsEffects;
class Crystal;
class CrystalBlock;
class Destructible;
class Enemy;
class Equipment;
class EquipmentItem;
class Explosion;
class Fire;
class Game;
class GameCommands;
class Hero;
class Jumper;
class LuaContext;
class Map;
class MainLoop;
class Entities;
class Movement;
class Npc;
class Savegame;
class Separator;
class Sensor;
class Sprite;
class Stairs;
class Stream;
class StreamAction;
class Switch;
class Teletransporter;

/**
 * \brief Abstract class for all objects placed on a map.
 *
 * Example of entities include tiles, enemies, the hero,
 * non-playing characters, doors, chests, etc.
 * Each entity has:
 * - a bounding box that represents its position on the map (a rectangle),
 * - a layer on the map,
 * - an origin point, relative to the rectangle's top-left corner.
 * Some entities can also have a name, a movement and some sprites.
 */
class SOLARUS_API Entity: public ExportableToLua {

  public:

    struct NamedSprite {
      std::string name;
      SpritePtr sprite;
      bool removed = false;
    };

    // Destruction.
    virtual ~Entity();
    void remove_from_map();
    virtual void notify_being_removed();
    bool is_being_removed() const;

    /**
     * \brief Returns the type of entity.
     * \return The type of entity.
     */
    virtual EntityType get_type() const = 0;
    virtual const std::string& get_lua_type_name() const override;
    bool is_hero() const;
    virtual bool is_ground_observer() const;
    Point get_ground_point() const;
    bool is_ground_modifier() const;
    virtual Ground get_modified_ground() const;
    virtual bool can_be_drawn() const;
    bool is_drawn_in_y_order() const;
    void set_drawn_in_y_order(bool drawn_in_y_order);
    virtual bool is_drawn_at_its_position() const;

    virtual void notify_command_pressed(GameCommand command);
    virtual void notify_command_released(GameCommand command);

    // adding to a map
    bool is_on_map() const;
    void set_map(Map& map);
    Map& get_map() const;
    virtual void notify_map_started();
    virtual void notify_creating();
    virtual void notify_created();
    virtual void notify_map_opening_transition_finished();
    virtual void notify_tileset_changed();
    Game& get_game();
    const Game& get_game() const;

    // position in the map
    int get_layer() const;
    void set_layer(int layer);
    Ground get_ground_below() const;

    int get_x() const;
    int get_y() const;
    void set_x(int x);
    void set_y(int y);
    Point get_xy() const;
    void set_xy(const Point& xy);
    void set_xy(int x, int y);
    Point get_displayed_xy() const;

    int get_width() const;
    int get_height() const;
    Size get_size() const;
    void set_size(int width, int height);
    void set_size(const Size& size);
    const Rectangle& get_bounding_box() const;
    void set_bounding_box(const Rectangle& bounding_box);
    Rectangle get_extended_bounding_box(int margin) const;
    virtual Rectangle get_max_bounding_box() const;
    void notify_bounding_box_changed();
    const Point& get_origin() const;
    void set_origin(int x, int y);
    void set_origin(const Point& origin);
    int get_top_left_x() const;
    int get_top_left_y() const;
    Point get_top_left_xy() const;
    void set_top_left_x(int x);
    void set_top_left_y(int y);
    void set_top_left_xy(int x, int y);
    void set_top_left_xy(const Point& xy);

    virtual Point get_facing_point() const;
    Point get_touching_point(int direction) const;
    Point get_center_point() const;

    bool is_aligned_to_grid() const;
    bool is_aligned_to_grid_x() const;
    bool is_aligned_to_grid_y() const;
    void set_aligned_to_grid();
    void set_aligned_to_grid_x();
    void set_aligned_to_grid_y();

    int get_optimization_distance() const;
    int get_optimization_distance2() const;
    void set_optimization_distance(int distance);

    bool is_enabled() const;
    void set_enabled(bool enable);
    virtual void notify_enabled(bool enabled);

    // properties
    const std::string& get_name() const;
    void set_name(const std::string& name);
    bool has_name() const;
    bool has_prefix(const std::string& prefix) const;
    int get_direction() const;
    void set_direction(int direction);
    virtual void notify_direction_changed();

    // Sprites.
    bool has_sprite() const;
    SpritePtr get_sprite(const std::string& sprite_name = "") const;
    std::vector<SpritePtr> get_sprites() const;
    std::vector<NamedSprite> get_named_sprites() const;
    SpritePtr create_sprite(
        const std::string& animation_set_id,
        const std::string& sprite_name = ""
    );
    bool remove_sprite(Sprite& sprite);
    void clear_sprites();
    bool bring_sprite_to_back(Sprite& sprite);
    bool bring_sprite_to_front(Sprite& sprite);
    std::string get_default_sprite_name() const;
    void set_default_sprite_name(const std::string& default_sprite_name);
    virtual void notify_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame);
    virtual void notify_sprite_animation_finished(Sprite& sprite, const std::string& animation);
    bool is_visible() const;
    void set_visible(bool visible);
    void set_animation_ignore_suspend(bool ignore_suspend);

    // Movement.
    const std::shared_ptr<Movement>& get_movement();
    std::shared_ptr<const Movement> get_movement() const;
    void set_movement(const std::shared_ptr<Movement>& movement);
    void clear_movement();
    bool are_movement_notifications_enabled() const;
    void set_movement_notifications_enabled(bool notify);
    bool has_stream_action() const;
    const StreamAction* get_stream_action() const;
    StreamAction* get_stream_action();
    void start_stream_action(std::unique_ptr<StreamAction> stream_action);
    void stop_stream_action();

    virtual void notify_obstacle_reached();
    virtual void notify_position_changed();
    virtual void notify_layer_changed();
    virtual void notify_ground_below_changed();
    virtual void notify_movement_changed();
    virtual void notify_movement_finished();
    virtual void notify_moving_by(Entity& entity);
    virtual void notify_moved_by(Entity& entity);

    Entity* get_facing_entity();
    const Entity* get_facing_entity() const;
    void set_facing_entity(Entity* facing_entity);
    virtual void notify_facing_entity_changed(Entity* facing_entity);
    static const Point& direction_to_xy_move(int direction8);

    // geometry
    bool overlaps(const Rectangle& rectangle) const;
    bool overlaps(const Point& point) const;
    bool overlaps(int x, int y) const;
    bool overlaps(const Entity& other) const;
    bool overlaps_camera() const;
    bool is_origin_point_in(const Rectangle& rectangle) const;
    bool is_facing_point_in(const Rectangle& rectangle) const;
    bool is_touching_point_in(const Rectangle& rectangle, int direction) const;
    bool is_center_in(const Rectangle& rectangle) const;

    double get_angle(int x, int y) const;
    double get_angle(const Entity& other) const;
    double get_angle(
        const Entity& other,
        const Sprite* this_sprite,
        const Sprite* other_sprite) const;
    int get_distance(int x, int y) const;
    int get_distance(const Point& point) const;
    int get_distance(const Entity& other) const;
    int get_distance_to_camera() const;
    int get_distance_to_camera2() const;
    bool is_in_same_region(const Entity& other) const;
    bool is_in_same_region(const Point& xy) const;

    // Collisions.
    bool is_detector() const;
    void set_collision_modes(int collision_modes);
    void add_collision_mode(CollisionMode collision_mode);
    bool has_collision_mode(CollisionMode collision_mode);
    void enable_pixel_collisions();

    bool has_layer_independent_collisions() const;
    void set_layer_independent_collisions(bool independent);

    // Detecting other entities.
    void check_collision(Entity& entity);
    void check_collision(Entity& entity, Sprite& sprite);
    bool test_collision(Entity& entity, CollisionMode collision_mode);
    bool test_collision_rectangle(const Entity& entity) const;
    bool test_collision_inside(const Entity& entity) const;
    bool test_collision_origin_point(const Entity& entity) const;
    bool test_collision_facing_point(const Entity& entity) const;
    bool test_collision_touching(const Entity& entity) const;
    bool test_collision_center(const Entity& entity) const;
    bool test_collision_sprites(Entity& entity);
    virtual bool test_collision_custom(Entity& entity);

    // Being detected by other entities.
    void check_collision_with_detectors();
    void check_collision_with_detectors(Sprite& sprite);

    virtual void check_position();
    virtual void notify_collision_with_destructible(Destructible& destructible, CollisionMode collision_mode);
    virtual void notify_collision_with_teletransporter(Teletransporter& teletransporter, CollisionMode collision_mode);
    virtual void notify_collision_with_stream(Stream& stream, int dx, int dy);
    virtual void notify_collision_with_stairs(Stairs& stairs, CollisionMode collision_mode);
    virtual void notify_collision_with_jumper(Jumper& jumper, CollisionMode collision_mode);
    virtual void notify_collision_with_sensor(Sensor& sensor, CollisionMode collision_mode);
    virtual void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    virtual void notify_collision_with_switch(Switch& sw, Sprite& sprite_overlapping);
    virtual void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode);
    virtual void notify_collision_with_crystal(Crystal& crystal, Sprite& sprite_overlapping);
    virtual void notify_collision_with_chest(Chest& chest);
    virtual void notify_collision_with_block(Block& block);
    virtual void notify_collision_with_separator(Separator& separator, CollisionMode collision_mode);
    virtual void notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode);
    virtual void notify_collision_with_explosion(Explosion& explosion, CollisionMode collision_mode);
    virtual void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping);
    virtual void notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping);
    virtual void notify_collision_with_enemy(Enemy& enemy);
    virtual void notify_collision_with_enemy(Enemy& enemy, Sprite& enemy_sprite, Sprite& this_sprite);
    virtual void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed);

    // Interactions.
    virtual bool notify_action_command_pressed();
    virtual bool notify_interaction_with_item(EquipmentItem& item);
    virtual bool start_movement_by_hero();
    virtual void stop_movement_by_hero();
    virtual std::string get_sword_tapping_sound();

    // Obstacles.
    virtual bool is_obstacle_for(Entity& other);
    virtual bool is_obstacle_for(Entity& other, const Rectangle& candidate_position);
    bool is_ground_obstacle(Ground ground) const;
    virtual bool is_hero_obstacle(Hero& hero);
    virtual bool is_block_obstacle(Block& block);
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    virtual bool is_stream_obstacle(Stream& stream);
    virtual bool is_stairs_obstacle(Stairs& stairs);
    virtual bool is_sensor_obstacle(Sensor& sensor);
    virtual bool is_switch_obstacle(Switch& sw);
    virtual bool is_raised_block_obstacle(CrystalBlock& raised_block);
    virtual bool is_crystal_obstacle(Crystal& crystal);
    virtual bool is_npc_obstacle(Npc& npc);
    virtual bool is_enemy_obstacle(Enemy& enemy);
    virtual bool is_jumper_obstacle(Jumper& jumper, const Rectangle& candidate_position);
    virtual bool is_destructible_obstacle(Destructible& destructible);
    virtual bool is_separator_obstacle(Separator& separator);
    virtual bool is_sword_ignored() const;

    // Game loop.
    bool is_suspended() const;
    virtual void set_suspended(bool suspended);
    virtual void update();
    virtual void draw_on_map();

    // Easy access to various game objects.
    Entities& get_entities();
    const Entities& get_entities() const;
    Equipment& get_equipment();
    const Equipment& get_equipment() const;
    CommandsEffects& get_commands_effects();
    GameCommands& get_commands();
    Savegame& get_savegame();
    const Savegame& get_savegame() const;
    Hero& get_hero();

    /**
     * \name State.
     *
     * These functions provide information about the entity's internal state
     * and allow to start actions which may modify this state.
     * Actions can be triggered by equipment items, entities or scripts.
     */
    class State;                                /**< base class for all states */

    State& get_state() const;
    void set_state(State* state);

    std::string get_state_name() const;
    void update_state();

  protected:

    // Creation.
    Entity(
        const std::string& name,
        int direction,
        int layer,
        const Point& xy,
        const Size& size
    );

    // No copy constructor or assignment operator.
    Entity(const Entity& other) = delete;
    Entity& operator=(const Entity& other) = delete;

    uint32_t get_when_suspended() const;

    // Ground.
    void update_ground_observers();
    void update_ground_below();

    // Collisions.
    virtual void notify_collision(
        Entity& entity_overlapping, CollisionMode collision_mode);
    virtual void notify_collision(
        Entity& other_entity, Sprite& this_sprite, Sprite& other_sprite);

    // Obstacles.
    virtual bool is_traversable_obstacle() const;
    virtual bool is_wall_obstacle() const;
    virtual bool is_low_wall_obstacle() const;
    virtual bool is_grass_obstacle() const;
    virtual bool is_shallow_water_obstacle() const;
    virtual bool is_deep_water_obstacle() const;
    virtual bool is_ice_obstacle() const;
    virtual bool is_hole_obstacle() const;
    virtual bool is_lava_obstacle() const;
    virtual bool is_prickle_obstacle() const;
    virtual bool is_ladder_obstacle() const;

  private:

    void finish_initialization();
    void clear_old_movements();
    void clear_old_sprites();

    MainLoop* main_loop;                        /**< The Solarus main loop. */
    Map* map;                                   /**< The map where this entity is, or nullptr
                                                 * (automatically set by class MapEntities after adding the entity to the map) */

    int layer;                                  /**< Layer of the entity on the map.
                                                 * The layer is constant for the tiles and can change for the hero and the dynamic entities. */

    Rectangle bounding_box;                     /**< This rectangle represents the position of the entity of the map and is
                                                 * used for the collision tests. It corresponds to the bounding box of the entity.
                                                 * It can be different from the sprite's rectangle of the entity.
                                                 * For example, the hero's bounding box is a 16*16 rectangle, but its sprite may be
                                                 * a 24*32 rectangle. */

    Ground ground_below;                        /**< Kind of ground under this entity: grass, shallow water, etc.
                                                 * Only used by entities sensible to their ground. */

    Point origin;                               /**< Coordinates of the origin point of the entity,
                                                 * relative to the top-left corner of its rectangle.
                                                 * Remember that when you call get_x() and get_y(), you get the coordinates
                                                 * of the origin point on the map, not the coordinates of the rectangle's
                                                 * top-left corner.
                                                 * This is useful because the top-left corner of the entity's bounding box does
                                                 * not represent the actual entity's coordinates and does not match necessarily
                                                 * the sprite's rectangle. */

    // other data, used for some kinds of entities only

    std::string name;                           /**< Name of the entity or an empty string.
                                                 * The name uniquely identifies the entity in the map. */

    int direction;                              /**< direction of the entity, not used for all kinds of entities */

    std::vector<NamedSprite>
        sprites;                                /**< Sprites representing the entity. */
    std::string default_sprite_name;            /**< Name of the sprite to get in get_sprite() without parameter. */
    bool visible;                               /**< Whether this entity's sprites are currently displayed. */
    bool drawn_in_y_order;                      /**< Whether this entity is drawn in Y order or in Z order. */
    std::shared_ptr<Movement> movement;         /**< Movement of the entity.
                                                 * nullptr indicates that the entity has no movement. */
    std::vector<std::shared_ptr<Movement>>
        old_movements;                          /**< Old movements to destroy as soon as possible. */
    bool movement_notifications_enabled;        /**< Whether entity:on_position_changed() and friends should be called. */
    Entity* facing_entity;                      /**< The detector in front of this entity if any. */
    int collision_modes;                        /**< Collision modes detected by entity
                                                 * (can be an OR combination of CollisionMode values). */
    bool layer_independent_collisions;          /**< Whether this entity detects collisions on all layers. */

    std::unique_ptr<StreamAction>
        stream_action;                          /**< The stream effect currently applied if any. */

    // state
    std::unique_ptr<State> state;               /**< The current internal state */
    std::list<std::unique_ptr<State>>
        old_states;                             /**< Previous state objects to delete as soon as possible. */

    bool initialized;                           /**< Whether all initializations were done. */
    bool being_removed;                         /**< indicates that the entity is not valid anymore because it is about to be removed */
    bool enabled;                               /**< indicates that the entity is enabled
                                                 * (if not, it will not be displayed and collisions will not be notified) */

    bool suspended;                             /**< indicates that the animation and movement of this entity are suspended */
    uint32_t when_suspended;                    /**< indicates when this entity was suspended */

    int optimization_distance;                  /**< Above this distance from the visible area,
                                                 * the engine may skip updates (0 means infinite). */
    int optimization_distance2;                 /**< Square of optimization_distance. */
    static constexpr int
        default_optimization_distance = 0;      /**< Default value. */

};

/**
 * \brief Returns whether this entity is enabled.
 * \return true if this entity is enabled
 */
inline bool Entity::is_enabled() const {
  return enabled;
}

/**
 * \brief Returns true if this entity is about to be deleted.
 *
 * When this function returns true, the entity is not
 * considered to be on the map anymore.
 *
 * \return true if this entity is about to be deleted
 */
inline bool Entity::is_being_removed() const {
  return being_removed;
}

/**
 * \brief Returns whether or not this entity's bounding box overlaps a specified rectangle.
 * \param rectangle the rectangle to check
 * \return true if this entity's bounding box overlaps the specified rectangle
 */
inline bool Entity::overlaps(const Rectangle& rectangle) const {
  return bounding_box.overlaps(rectangle);
}

/**
 * \brief Returns whether or not a point overlaps this entity's bounding box.
 * \param point point to check
 * \return true if the point is in this entity's bounding box
 */
inline bool Entity::overlaps(const Point& point) const {
  return bounding_box.contains(point);
}

/**
 * \brief Returns whether or not a point overlaps this entity's bounding box.
 * \param x x coordinate of the point to check
 * \param y y coordinate of the point to check
 * \return true if the point is in this entity's bounding box
 */
inline bool Entity::overlaps(int x, int y) const {
  return bounding_box.contains(x, y);
}

/**
 * \brief Returns whether or not this entity's bounding box overlaps
 * another entity's bounding box.
 * \param other another entity
 * \return true if this entity's bounding box overlaps the other entity's bounding box
 */
inline bool Entity::overlaps(const Entity& other) const {
  return overlaps(other.get_bounding_box());
}

}

#endif

