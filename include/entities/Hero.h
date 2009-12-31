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
#ifndef ZSDX_HERO_H
#define ZSDX_HERO_H

#include "Common.h"
#include "entities/MapEntity.h"
#include "entities/EnemyAttack.h"
#include "entities/Ground.h"
#include "Controls.h"
#include "InventoryItemId.h"

/**
 * The hero's entity.
 * It is animated and can be controlled with an 8 directions system.
 * This class handles the hero's actions: the movements and the animation of his sprites.
 */
class Hero: public MapEntity {

  public:

    /**
     * Possible states of the hero.
     * The state is considered only when the game is not suspended.
     * The hero can move (and walk on teletransporters) in states FREE, PUSHING, CARRYING, SWORD_LOADING and SWIMMING.
     * He can swing his sword in states FREE, PUSHING, CARRYING and SWORD_SWINGING.
     * He hero can be hurt in states <= SPIN_ATTACK.
     */
    enum State {

      // the hero can move in these states
      FREE,                        /**< the hero is free to move (stopped or walking) */
      CARRYING,                    /**< the hero can walk but he is carrying a pot or a bush */
      SWORD_LOADING,               /**< the hero can walk but his sword is loading for a spin attack */
      SWIMMING,                    /**< the hero is swimming in deep water */

      // the hero can change his direction but cannot move in these states
      PUSHING,                     /**< the hero is trying to push an obstacle */
      SWORD_TAPPING,               /**< the hero is tapping his sword on a wall */
      PULLING,                     /**< the hero is pulling an object */
      GRABBING,                    /**< the hero is grabbing an object and can pull it */
      CONVEYOR_BELT,               /**< the hero is being moved by a conveyor belt */

      // the hero cannot move in these states
      SWORD_SWINGING,              /**< the hero is swinging his sword */
      SPIN_ATTACK,                 /**< the hero is releasing a spin attack */
      LIFTING,                     /**< the hero is lifting an destroyable item (a pot, a bush, etc.) */
      BRANDISHING_TREASURE,        /**< the hero is brandishing a treasure */
      JUMPING,                     /**< the hero is jumping */
      HURT,                        /**< the hero is hurt */
      PLUNGING,                    /**< the hero is plunging into water */
      FALLING,                     /**< the hero is falling into a hole */
      RETURNING_TO_SOLID_GROUND,   /**< the hero is returning towards solid ground (e.g. after he drowned
				    * in deep water or falled into a hole) */
      VICTORY,                     /**< the hero is make a victory sequence with his sword */
      USING_INVENTORY_ITEM,        /**< the hero is currently using an item from the inventory */
      FREEZED,                     /**< the hero cannot move for various possible reasons,
				    * including an instruction from the script */
    };

  private:

    // sprites
    Equipment *equipment;                                       /**< equipment of the player */
    HeroSprites *sprites;                                       /**< the hero's sprites (note that we don't use the sprites structure from MapEntity) */

    // movement
    PlayerMovement *normal_movement;
    static const int walking_speed;                             /**< speed of the player movement */
    bool move_tried;                                            /**< indicates that the hero just tried to move */
    int old_x;                                                  /**< x position of the hero before the last move try */
    int old_y;                                                  /**< y position of the hero before the last move try */

    // state
    State state;                   /**< current state of the hero (considered only when the game is not suspended) */
    Detector *facing_entity;       /**< the entity just in front of the hero */

    int counter;                   /**< counter incremented every 100 ms in certain conditions:
				    * - in state FREE: counts for how long the hero is trying to walk
				    * to a wall (animation pushing is triggered at 800 ms)
				    * - in state SWORD_LOADING: counts for how long the hero is loading
				    * his sword (the spin attack is possible after 1000 ms) */

    uint32_t next_counter_date;      /**< when the counter will be incremented */

    // pushing
    uint16_t pushing_direction_mask; /**< direction of the hero's movement when pushing
				      * (0xFFFF indicates that he is currently not trying to push) */
    Detector *grabbed_entity;        /**< the entity the hero is pushing or pulling */

    // conveyor belt
    bool on_conveyor_belt;         /**< indicates that the hero's rectangle is currently overlapping a conveyor belt,
				    * (even if the collision is not enough to go to state CONVEYOR_BELT and move the hero) */
    bool conveyor_belt_snapping;   /**< in state CONVEYOR_BELT, indicates that the hero is currently moving towards the
				    * center of a conveyor belt, before following the conveyor belt's direction */
    ConveyorBelt *current_conveyor_belt; /**< the current conveyor belt in state CONVEYOR_BELT */

    // sword loading
    bool sword_loaded;             /**< in state SWORD_LOADING, becomes true when the spin attack is possible */
    uint32_t next_hit_sound_date;  /**< when the sword hit sound has to be played next time */

    // lift and carry an object
    CarriedItem *lifted_item;      /**< item being lifted or carried */
    CarriedItem *thrown_item;      /**< item thrown and not broken yet */

    // brandish a treasure
    Treasure *treasure;            /**< the treasure being brandished (if any) */

    // victory 
    uint32_t end_victory_date;     /**< date when the victory animation should stop */

    // jump
    int jump_y;                    /**< height of the hero's sprite when jumping, relative to its shadow on the ground */
    Layer layer_after_jump;        /**< the layer to set when the jump movement is finished */

    // return to solid ground
    Rectangle last_solid_ground_coords;    /**< coordinates of the last hero position on a ground
					    * where he can walk (e.g. before jumping or falling into a hole) */
    Layer last_solid_ground_layer;         /**< layer of the last hero position a solid ground */
    Rectangle target_solid_ground_coords;  /**< coordinates of the position where the hero will go if he falls
					    * into a hole (or some other bad ground), or (-1,-1) to indicate
					    * that the hero will just return to the last solid ground coordinates */
    Layer target_solid_ground_layer;       /**< layer of the place to go back when falling in some bad ground */

    // special ground under the hero
    Ground ground;                         /**< kind of ground under the hero: grass, shallow water, etc. */
    uint32_t next_ground_date;             /**< when something will happend with the ground sound next time */
    int hole_dx;                           /**< x direction of the movement when the hero is being attracted by a hole */
    int hole_dy;                           /**< y direction of the movement when the hero is being attracted by a hole */
    Teletransporter *hole_teletransporter; /**< a teletransporter in the current hole */

    // items
    InventoryItem *current_inventory_item; /**< the inventory item the player is currently using,
					    * or NULL if he is not using an item */
    uint32_t when_can_use_inventory_item;  /**< date when the player can use an inventory item next time */

    // update functions
    void update_position(void);
    void movement_just_changed(void);

    void try_snap_to_facing_entity(void);

    void action_key_pressed(void);
    void sword_key_pressed(void);
    void item_key_pressed(int slot);
    void arrow_pressed(int direction);
    void arrow_released(int direction);

    // the hero's state
    void set_state(State state);

    void start_ground(void);
    void update_ground(void);

    bool can_start_inventory_item(InventoryItemId item_id);
    void start_inventory_item(InventoryItemId item_id);
    void update_inventory_item(void);

    void update_sword_swinging(void);
    void start_sword_loading(void);
    void update_sword_loading(void);
    void start_sword_tapping(void);
    void update_sword_tapping(void);
    void start_spin_attack(void);
    void update_spin_attack(void);
    bool can_start_sword(void);

    void stop_carrying(void);
    void start_throwing(void);
    void set_suspended_carried_items(bool suspended);
    void update_carried_items(void);
    void display_carried_items(void);
    void destroy_carried_items(void);

    void start_pushing(void);
    void update_pushing(void);
    void start_grabbing(void);
    void start_pulling(void);
    void update_grabbing_pulling(void);
    void update_moving_grabbed_entity(void);

    void update_conveyor_belt(void);

    void update_treasure(void);
    void display_treasure(void);

    void update_jumping(void);
    bool can_be_hurt(void);
    void update_hurt(void);
    bool can_start_gameover_sequence(void);

    void start_deep_water(void);
    void start_plunging(void);
    void update_plunging(void);
    void start_swimming(void);
    void stop_swimming(void);
    void start_hole(void);
    void start_falling(void);
    void update_falling(void);

    void update_victory(void);
    void update_freezed(void);

    void start_returning_to_solid_ground(const Rectangle &target_xy, Layer target_layer);
    void update_returning_to_solid_ground(void);

  public:

    // creation and destruction
    Hero(Equipment *equipment);
    ~Hero(void);

    // game loop
    void update(void);
    void display_on_map(void);
    void set_suspended(bool suspended);

    // features
    EntityType get_type(void);
    bool can_be_obstacle(void);
    bool can_detect_entities(void);
    bool can_be_displayed(void);
    bool is_displayed_in_y_order(void);

    // sprites
    int get_animation_direction(void);
    void set_animation_direction(int direction);
    bool is_animation_finished(void);
    void rebuild_equipment(void);

    // movement
    PlayerMovement * get_normal_movement(void);
    int get_wanted_movement_direction(void);
    int get_wanted_movement_direction8(void);
    int get_real_movement_direction(void);
    int get_real_movement_direction8(void);
    bool is_moving_towards(int direction);
    const Rectangle get_facing_point(void);
    const Rectangle get_facing_point(int direction);
    Detector *get_facing_entity(void);
    void just_moved(void);
    void set_facing_entity(Detector *detector);
    bool is_facing_obstacle(void);
    void reset_movement(void);
    bool is_direction_locked(void);

    // map
    void set_map(Map *map);
    void set_map(Map *map, int initial_direction);
    void place_on_destination_point(Map *map);
    void opening_transition_finished(void);

    // state of the hero
    State get_state(void);
    Ground get_ground(void);
    void set_ground(Ground ground);
    bool is_ground_visible(void);
    void start_free(void);
    void start_sword(void);
    bool is_grabbing_or_pulling(void);
    bool is_moving_grabbed_entity(void);
    void grabbed_entity_collision(void);
    void stop_moving_grabbed_entity(void);
    void start_lifting(DestructibleItem *item_to_lift);
    void start_carrying(void);
    CarriedItem *get_lifted_item(void);
    void freeze(void);
    void unfreeze(void);
    void give_treasure(Treasure *treasure);
    void start_jumping(int direction, int length, bool with_collisions, bool with_sound);
    void start_jumping(int direction, int length, bool with_collisions, bool with_sound, Layer layer_after_jump);
    int get_jump_y(void);
    void hurt(MapEntity *source, int life_points, int magic_points);
    void get_back_from_death(void);
    void set_target_solid_ground_coords(const Rectangle &target_solid_ground_coords, Layer layer);
    void start_victory(void);

    // using items (functions called by the InventoryItem class)
    void start_boomerang(void);
    void start_bow(void);
    void start_pegasus_shoes(void);
    void update_pegasus_shoes(void);
    bool are_pegasus_shoes_finished(void);

    // keys
    void key_pressed(Controls::GameKey key);
    void key_released(Controls::GameKey key);

    // enemies and collisions
    bool is_obstacle_for(MapEntity *other);
    void notify_collision_with_enemy(Enemy *enemy);
    void notify_collision_with_enemy(Enemy *enemy, Sprite *enemy_sprite, Sprite *this_sprite);
    void just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed);
    bool is_stroke_by_sword(Detector *detector);

    void notify_collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode);
    void notify_collision_with_conveyor_belt(ConveyorBelt *conveyor_belt, int dx, int dy);
    void notify_collision_with_sensor(Sensor *sensor);
    void notify_collision_with_explosion(Explosion *explosion, Sprite *sprite_overlapping);
    void avoid_chest_collision(Chest *chest);
    bool is_on_raised_blocks(void);

    bool is_on_hole(void);
    bool is_water_obstacle(void);
    bool is_hole_obstacle(void);
    bool is_ladder_obstacle(void);
    bool is_teletransporter_obstacle(Teletransporter *teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt);
    bool is_sensor_obstacle(Sensor *sensor);
    bool is_raised_block_obstacle(CrystalSwitchBlock *raised_block);
    bool is_jump_sensor_obstacle(JumpSensor *jump_sensor);
};

#endif

