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
#ifndef SOLARUS_HERO_H
#define SOLARUS_HERO_H

#include "Common.h"
#include "entities/MapEntity.h"
#include "entities/EnemyAttack.h"
#include "entities/Ground.h"
#include "lowlevel/Rectangle.h"
#include "GameControls.h"

/**
 * @brief The hero's entity.
 *
 * This class represents the hero. It coordinates his position on the map and his state.
 * The hero is animated by several sprites that are handled by the HeroSprites class.
 */
class Hero: public MapEntity {

  private:

    // state
    class State;                    /**< base class for all states */
    class PlayerMovementState;      /**< base class for states whose movement is controlled by the player */
    class FreeState;                /**< the hero is free to move (stopped or walking) and can interact with entities */
    class CarryingState;            /**< the hero can walk but he is carrying a pot or a bush */
    class SwordLoadingState;        /**< the hero can walk but his sword is loading for a spin attack */
    class SwimmingState;            /**< the hero is swimming in deep water */
    class PushingState;             /**< the hero is trying to push an obstacle */
    class SwordTappingState;        /**< the hero is tapping his sword on a wall */
    class PullingState;             /**< the hero is pulling an object */
    class GrabbingState;            /**< the hero is grabbing an object and can pull it */
    class ConveyorBeltState;        /**< the hero is being moved by a conveyor belt */
    class SwordSwingingState;       /**< the hero is swinging his sword */
    class SpinAttackState;          /**< the hero is releasing a spin attack */
    class LiftingState;             /**< the hero is lifting an destroyable item (a pot, a bush, etc.) */
    class TreasureState;            /**< the hero is brandishing a treasure */
    class RunningState;             /**< the hero is running */
    class ForcedWalkingState;       /**< the hero is walking with a predetermined path */
    class JumpingState;             /**< the hero is jumping */
    class HurtState;                /**< the hero is hurt */
    class PlungingState;            /**< the hero is plunging into water */
    class FallingState;             /**< the hero is falling into a hole */
    class BackToSolidGroundState;   /**< the hero is getting back to solid ground (e.g. after he drowned
                                     * in deep water or falled into a hole) */
    class StairsState;              /**< the hero is being moved by stairs */
    class VictoryState;             /**< the hero is make a victory sequence with his sword */
    class InventoryItemState;       /**< the hero is currently using an item from the inventory */
    class BoomerangState;           /**< the hero is shooting a boomerang */
    class HookshotState;            /**< the hero is throwing his hookshot */
    class BowState;                 /**< the hero is shooting an arrow with a bow */
    class FreezedState;             /**< the hero cannot move for various possible reasons,
                                     * including an instruction from the script */

    State *state;                   /**< the current internal state */
    std::list<State*> old_states;   /**< previous state objects to delete as soon as possible */

    // sprites
    HeroSprites *sprites;           /**< the hero's sprites (note that we don't use the sprites structure from MapEntity) */

    // position
    static const int normal_walking_speed;   /**< default speed when walking */
    int walking_speed;                       /**< current walking speed */

    // state specific
    Teletransporter *delayed_teletransporter;   /**< a teletransporter that will be activated when the hero finishes
                                                  * a special behavior, such as falling into a hole or walking on stairs */
    bool on_conveyor_belt;          /**< indicates that the hero's rectangle is currently overlapping a conveyor belt
                                     * (even if the collision is not enough to take the conveyor belt and move the hero) */
    bool on_raised_blocks;          /**< indicates that the hero is currently on
                                     * raised crystal blocks */

    // ground
    Ground ground;                         /**< kind of ground under the hero: grass, shallow water, etc. */
    Rectangle last_solid_ground_coords;    /**< coordinates of the last hero position on a ground
                                            * where he can walk (e.g. before jumping or falling into a hole) */
    Layer last_solid_ground_layer;         /**< layer of the last hero position on a solid ground */
    Rectangle target_solid_ground_coords;  /**< coordinates of the position where the hero will go if he falls
                                            * into a hole (or some other bad ground), or (-1,-1) to indicate
                                            * that the hero will just return to the last solid ground coordinates */
    Layer target_solid_ground_layer;       /**< layer of the place to go back when falling in some bad ground */
    uint32_t next_ground_date;             /**< when something will happend with the ground next time (a sound or a movement) */
    Rectangle hole_dxy;                    /**< direction of the movement when the hero is being attracted by a hole */

    // state
    void set_state(State *state);
    void update_state();

    // position
    void update_movement();
    void try_snap_to_facing_entity();
    Teletransporter* get_delayed_teletransporter();

    // ground
    void update_ground();
    void notify_ground_changed();

    // life
    void check_gameover();

  public:

    /**
     * @name Creation and destruction.
     */
    Hero(Equipment &equipment);
    ~Hero();

    /**
     * @name Features.
     *
     * These functions, required by MapEntity, indicate
     * the main properties of this type of entity.
     */
    EntityType get_type();
    bool can_be_obstacle();
    bool can_detect_entities();
    bool can_be_displayed();
    bool is_displayed_in_y_order();

    /**
     * @name Game loop.
     *
     * Functions called by the game loop.
     */
    void update();
    void display_on_map();
    void set_suspended(bool suspended);
    void key_pressed(GameControls::GameKey key);
    void key_released(GameControls::GameKey key);

    /**
     * @name Sprites.
     *
     * Functions relative to the sprites.
     * The sprites are managed and displayed by the class HeroSprites.
     */
    HeroSprites& get_sprites();
    int get_animation_direction();
    void set_animation_direction(int direction);
    bool is_animation_finished();
    void rebuild_equipment();
    bool is_visible();
    bool is_shadow_visible();

    /**
     * @name Changing map.
     *
     * Functions called when the player goes to another map.
     */
    void set_map(Map &map);
    void set_map(Map &map, int initial_direction);
    void notify_map_started();
    void place_on_destination_point(Map& map, const Rectangle& previous_map_location);
    void notify_map_opening_transition_finished();

    /**
     * @name Position.
     *
     * These function provide information about the position of
     * the hero relative to other entities, and about
     * what is in front of him (we call this the "facing point"). 
     */
    const Rectangle get_facing_point();
    const Rectangle get_facing_point(int direction4);
    void notify_facing_entity_changed(Detector* facing_entity);
    bool is_facing_obstacle();
    bool is_facing_point_on_obstacle();
    bool is_facing_direction4(int direction4);
    bool is_facing_direction8(int direction8);
    bool is_on_raised_blocks();
    Stairs* get_stairs_overlapping();

    /**
     * @name Movement.
     *
     * Information about the movement of the hero.
     * The hero is controlled by the player most of the time,
     * and sometimes he is controlled automatically or he
     * cannot move.
     */
    bool can_control_movement();
    bool can_control_direction();
    int get_normal_walking_speed();
    int get_walking_speed();
    void set_walking_speed(int walking_speed);
    int get_wanted_movement_direction8();
    int get_real_movement_direction8();
    bool is_moving_towards(int direction4);
    bool is_direction_locked();
    void notify_movement_tried(bool success);
    void notify_position_changed();
    void notify_layer_changed();
    void notify_movement_changed();
    void reset_movement();

    /**
     * @name Ground under the hero.
     *
     * Functions to handle the ground of the tiles under the hero.
     * Depending on the kind of ground, a special sprite may be displayed under him (grass, shallow water)
     * or something bad can happen (deep water, holes).
     */
    Ground get_ground();
    void set_ground(Ground ground);
    bool is_ground_visible();
    Ground get_tile_ground();
    const Rectangle get_ground_point();
    void set_target_solid_ground_coords(const Rectangle &target_solid_ground_coords, Layer layer);

    /**
     * @name Obstacles.
     *
     * Information about what is considered as an obstacle for the hero.
     */
    bool is_obstacle_for(MapEntity &other);
    bool is_deep_water_obstacle();
    bool is_hole_obstacle();
    bool is_lava_obstacle();
    bool is_prickle_obstacle();
    bool is_ladder_obstacle();
    bool is_block_obstacle(Block &block);
    bool is_teletransporter_obstacle(Teletransporter &teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt);
    bool is_stairs_obstacle(Stairs &stairs);
    bool is_sensor_obstacle(Sensor &sensor);
    bool is_raised_block_obstacle(CrystalBlock &raised_block);
    bool is_jumper_obstacle(Jumper& jumper);

    /**
     * @name Collisions.
     *
     * Handle collisions between the hero and other entities.
     */
    void check_position();
    void notify_collision_with_destructible_item(DestructibleItem &destructible_item, CollisionMode collision_mode);
    void notify_collision_with_enemy(Enemy &enemy);
    void notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite);
    void notify_collision_with_teletransporter(Teletransporter &teletransporter, CollisionMode collision_mode);
    void notify_collision_with_conveyor_belt(ConveyorBelt &conveyor_belt, int dx, int dy);
    void notify_collision_with_stairs(Stairs &stairs, CollisionMode collision_mode);
    void notify_collision_with_jumper(Jumper &jumper);
    void notify_collision_with_sensor(Sensor &sensor, CollisionMode collision_mode);
    void notify_collision_with_switch(Switch &sw, CollisionMode collision_mode);
    void notify_collision_with_switch(Switch &sw, Sprite &sprite_overlapping);
    void notify_collision_with_crystal(Crystal &crystal, CollisionMode collision_mode);
    void notify_collision_with_crystal(Crystal &crystal, Sprite &sprite_overlapping);
    void notify_collision_with_chest(Chest& chest);
    void notify_collision_with_block(Block& block);
    void notify_collision_with_bomb(Bomb& bomb, CollisionMode collision_mode);
    void notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping);
    void avoid_collision(MapEntity &entity, int direction);
    bool is_striking_with_sword(Detector &detector);

    /**
     * @name Enemies.
     *
     * Attacking enemies or getting hurt by them.
     */
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim, EnemyReaction::Reaction& result, bool killed);
    int get_sword_damage_factor();
    void hurt(MapEntity& source, int life_points, int magic_points);
    void hurt(const Rectangle& source_xy, int life_points, int magic_points);
    void get_back_from_death();

    /**
     * @name State.
     *
     * These functions provide information about the hero's internal state
     * and allow to start actions which may modify this state.
     * Actions can be triggered by inventory items, entities or scripts.
     */
    bool is_free();
    bool is_using_inventory_item();
    InventoryItem& get_current_inventory_item();
    bool is_grabbing_or_pulling();
    bool is_moving_grabbed_entity();
    void notify_grabbed_entity_collision();

    void start_deep_water();
    void start_hole();
    void start_lava();
    void start_prickle(uint32_t delay);

    void start_next_state();
    void start_free();
    void start_treasure(const Treasure &treasure);
    void start_forced_walking(const std::string &path, bool loop, bool ignore_obstacles);
    void start_jumping(int direction8, int length, bool ignore_obstacles,
        bool with_sound, uint32_t movement_delay = 0);
    void start_freezed();
    void start_victory();
    void start_lifting(CarriedItem* item_to_lift);
    void start_running();
    void start_grabbing();
    void start_boomerang();
    void start_bow();
    void start_hookshot();
    void start_state_from_ground();
};

#endif

