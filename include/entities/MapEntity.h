/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_MAP_ENTITY_H
#define SOLARUS_MAP_ENTITY_H

#include "Common.h"
#include "lua/ExportableToLua.h"
#include "entities/EntityType.h"
#include "entities/Layer.h"
#include "entities/CollisionMode.h"
#include "entities/EnemyAttack.h"
#include "entities/EnemyReaction.h"
#include "lowlevel/Rectangle.h"
#include <list>

struct lua_State;

/**
 * @brief Abstract class for all objects placed on a map.
 *
 * Example of entities include tiles, enemies, the hero,
 * interactive objects, doors, chests, etc.
 * Each entity has:
 * - a bounding box that represents its position on the map (a rectangle),
 * - a layer on the map
 * - an origin point, relative to the rectangle's top-left corner
 * Some entities can also have a name, a movement and some sprites.
 */
class MapEntity: public ExportableToLua {

  public:

    static const Rectangle directions_to_xy_moves[8];  /**< converts a direction (0 to 7) into a one-pixel xy move */

  private:

    MainLoop* main_loop;                        /**< The Solarus main loop. */
    Map* map;                                   /**< The map where this entity is, or NULL
                                                 * (automatically set by class MapEntities after adding the entity to the map) */

    Layer layer;                                /**< Layer of the entity: LAYER_LOW, LAYER_INTERMEDIATE or LAYER_HIGH.
                                                 * The layer is constant for the tiles and can change for the hero and the dynamic entities.
                                                 * The layer is private to make sure the set_layer() function is always called. */

    Rectangle bounding_box;                     /**< This rectangle represents the position of the entity of the map and is
                                                 * used for the collision tests. It corresponds to the bounding box of the entity.
                                                 * It can be different from the sprite's rectangle of the entity.
                                                 * For example, the hero's bounding box is a 16*16 rectangle, but its sprite may be
                                                 * a 24*32 rectangle. */

    Rectangle origin;                           /**< Coordinates of the origin point of the entity,
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

    std::list<Sprite*> sprites;                 /**< sprites representing the entity;
                                                 * note that some entities manage their sprites themselves rather than using this field */
    std::list<Sprite*> old_sprites;             /**< sprites to remove and destroy as soon as possible */
    bool visible;                               /**< indicates that this entity's sprites are currently displayed */
    Movement* movement;                         /**< movement of the entity, not used for all kinds of entities;
                                                 * NULL indicates that the entity has no movement */
    std::list<Movement*> old_movements;         /**< old movements to destroy as soon as possible */
    Detector* facing_entity;                    /**< the detector in front of this entity (if any) */

    // entity state
    bool being_removed;                         /**< indicates that the entity is not valid anymore because it is about to be removed */
    bool enabled;                               /**< indicates that the entity is enabled
                                                 * (if not, it will not be displayed and collisions will not be notified) */
    bool waiting_enabled;                       /**< indicates that the entity will be enabled as soon as the hero stops overlapping it */

    int optimization_distance;                  /**< above this distance from the visible area,
                                                 * the entity is suspended (0 means infinite) */
    static const int
        default_optimization_distance = 400;    /**< default value */

  protected:

    bool suspended;                             /**< indicates that the animation and movement of this entity are suspended */
    uint32_t when_suspended;                    /**< indicates when this entity was suspended */

    // creation
    MapEntity();
    MapEntity(Layer layer, int x, int y, int width, int height);
    MapEntity(const std::string& name, int direction, Layer layer, int x, int y, int width, int height);

    void clear_old_movements();
    void clear_old_sprites();

    void set_direction(int direction);

    // easy access to various game objects
    LuaContext& get_lua_context() const;
    MapEntities& get_entities() const;
    Equipment& get_equipment() const;
    KeysEffect& get_keys_effect() const;
    GameCommands& get_commands() const;
    DialogBox& get_dialog_box() const;
    Savegame& get_savegame() const;
    Hero& get_hero() const;

  public:

    // destruction
    virtual ~MapEntity();
    void remove_from_map();
    virtual void notify_being_removed();
    bool is_being_removed();

    /**
     * @brief Returns the type of entity.
     * @return the type of entity
     */
    virtual EntityType get_type() = 0;  // TODO make const
    bool is_hero();
    virtual bool is_detector();
    virtual bool can_be_obstacle();
    virtual bool can_be_drawn();
    virtual bool is_drawn_in_y_order();

    // adding to a map
    bool is_on_map();
    virtual void set_map(Map &map);
    Map& get_map();
    virtual void notify_map_started();
    virtual void notify_map_opening_transition_finished();
    virtual void notify_tileset_changed();
    Game& get_game() const;

    // position in the map
    Layer get_layer();
    void set_layer(Layer layer);

    int get_x();
    int get_y();
    void set_x(int x);
    void set_y(int y);
    const Rectangle get_xy();
    void set_xy(const Rectangle &xy);
    void set_xy(int x, int y);
    const Rectangle get_displayed_xy();

    int get_width();
    int get_height();
    const Rectangle& get_size();
    void set_size(int width, int height);
    void set_size(const Rectangle &size);
    const Rectangle& get_bounding_box();
    void set_bounding_box_from_sprite();
    void set_bounding_box(const Rectangle &bounding_box);
    const Rectangle& get_origin();
    void set_origin(int x, int y);
    void set_origin(const Rectangle &origin);
    int get_top_left_x();
    int get_top_left_y();
    void set_top_left_x(int x);
    void set_top_left_y(int y);
    void set_top_left_xy(int x, int y);

    virtual const Rectangle get_facing_point();
    virtual const Rectangle get_facing_point(int direction);
    const Rectangle get_center_point();

    bool is_aligned_to_grid();
    bool is_aligned_to_grid_x();
    bool is_aligned_to_grid_y();
    void set_aligned_to_grid();
    void set_aligned_to_grid_x();
    void set_aligned_to_grid_y();

    int get_optimization_distance();
    void set_optimization_distance(int distance);

    bool is_enabled();
    void set_enabled(bool enable);
    virtual void notify_enabled(bool enabled);

    // properties
    const std::string& get_name() const;
    bool has_prefix(const std::string &prefix);
    int get_direction();

    // sprites
    bool has_sprite();
    Sprite& get_sprite();
    std::list<Sprite*>& get_sprites();
    Sprite& create_sprite(const std::string& animation_set_id,
        bool enable_pixel_collisions = false);
    void remove_sprite(Sprite& sprite);
    void clear_sprites();
    virtual void notify_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame);
    virtual void notify_sprite_animation_finished(Sprite& sprite, const std::string& animation);
    virtual bool is_visible();
    virtual void set_visible(bool visible);
    void set_animation_ignore_suspend(bool ignore_suspend);

    // movement
    Movement* get_movement();
    void set_movement(Movement *movement);
    void clear_movement();

    virtual void notify_obstacle_reached();
    virtual void notify_position_changed();
    virtual void notify_layer_changed();
    virtual void notify_movement_changed();
    virtual void notify_movement_finished();
    Detector* get_facing_entity();
    void set_facing_entity(Detector* facing_entity);
    virtual void notify_facing_entity_changed(Detector* facing_entity);
    static const Rectangle& direction_to_xy_move(int direction8);

    // geometry
    bool overlaps(const Rectangle &rectangle);
    bool overlaps(int x, int y);
    bool overlaps(MapEntity &other);
    bool overlaps_camera();
    bool is_origin_point_in(const Rectangle &rectangle);
    bool is_facing_point_in(const Rectangle &rectangle);
    bool is_facing_point_in(const Rectangle &rectangle, int direction);
    bool is_center_in(const Rectangle &rectangle);

    double get_angle(int x, int y);
    double get_angle(MapEntity &other);
    int get_distance(int x, int y);
    int get_distance(MapEntity& other);
    int get_distance_to_camera();

    // collisions
    virtual bool has_layer_independent_collisions();

    void check_collision_with_detectors(bool with_pixel_precise);
    void check_collision_with_detectors(Sprite& sprite);

    virtual void notify_collision_with_enemy(Enemy &enemy);
    virtual void notify_collision_with_destructible(Destructible &destructible, CollisionMode collision_mode);
    virtual void notify_collision_with_teletransporter(Teletransporter &teletransporter, CollisionMode collision_mode);
    virtual void notify_collision_with_conveyor_belt(ConveyorBelt &conveyor_belt, int dx, int dy);
    virtual void notify_collision_with_stairs(Stairs &stairs, CollisionMode collision_mode);
    virtual void notify_collision_with_jumper(Jumper &jumper);
    virtual void notify_collision_with_sensor(Sensor &sensor, CollisionMode collision_mode);
    virtual void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    virtual void notify_collision_with_switch(Switch& sw, Sprite& sprite_overlapping);
    virtual void notify_collision_with_crystal(Crystal &crystal, CollisionMode collision_mode);
    virtual void notify_collision_with_crystal(Crystal &crystal, Sprite &sprite_overlapping);
    virtual void notify_collision_with_chest(Chest& chest);
    virtual void notify_collision_with_block(Block& block);
    virtual void notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode);
    virtual void notify_collision_with_explosion(Explosion& explosion, CollisionMode collision_mode);
    virtual void notify_collision_with_explosion(Explosion& explosion, Sprite &sprite_overlapping);
    virtual void notify_collision_with_fire(Fire& fire, Sprite& sprite_overlapping);
    virtual void notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite);
    virtual void notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
        EnemyReaction::Reaction& result, bool killed);

    virtual bool is_obstacle_for(MapEntity &other);
    virtual bool is_shallow_water_obstacle();
    virtual bool is_deep_water_obstacle();
    virtual bool is_hole_obstacle();
    virtual bool is_lava_obstacle();
    virtual bool is_prickle_obstacle();
    virtual bool is_ladder_obstacle();
    virtual bool is_hero_obstacle(Hero& hero);
    virtual bool is_block_obstacle(Block& block);
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    virtual bool is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt);
    virtual bool is_stairs_obstacle(Stairs& stairs);
    virtual bool is_sensor_obstacle(Sensor& sensor);
    virtual bool is_switch_obstacle(Switch& sw);
    virtual bool is_raised_block_obstacle(CrystalBlock& raised_block);
    virtual bool is_crystal_obstacle(Crystal& crystal);
    virtual bool is_npc_obstacle(NPC& npc);
    virtual bool is_enemy_obstacle(Enemy& enemy);
    virtual bool is_jumper_obstacle(Jumper& jumper);
    virtual bool is_destructible_obstacle(Destructible& destructible);
    virtual bool is_sword_ignored();

    // game loop
    bool is_suspended();
    virtual void set_suspended(bool suspended);
    virtual void update();
    bool is_drawn();
    virtual void draw_on_map();

    virtual const std::string& get_lua_type_name() const;
};

#endif

