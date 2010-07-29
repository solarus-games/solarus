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
 * The hero is animated by several sprites and is controlled with an instance of PlayerMovement.
 * This class handles the states and actions of the hero.
 * The sprites are handled by the HeroSprites class.
 */
class Hero: public MapEntity {

  public:

    /**
     * @brief Constants to identify the possible states of the hero.
     */
    enum StateId {

      // the hero can move in these states
      FREE,				/**< the hero is free to move (stopped or walking) */
      CARRYING,				/**< the hero can walk but he is carrying a pot or a bush */
      SWORD_LOADING,			/**< the hero can walk but his sword is loading for a spin attack */
      RUNNING,				/**< the hero is running */
      SWIMMING,				/**< the hero is swimming in deep water */

      // the hero can change his direction but cannot move in these states
      PUSHING,				/**< the hero is trying to push an obstacle */
      SWORD_TAPPING,			/**< the hero is tapping his sword on a wall */
      PULLING,				/**< the hero is pulling an object */
      GRABBING,				/**< the hero is grabbing an object and can pull it */
      CONVEYOR_BELT,			/**< the hero is being moved by a conveyor belt */

      // the hero cannot move in these states
      SWORD_SWINGING,			/**< the hero is swinging his sword */
      SPIN_ATTACK,			/**< the hero is releasing a spin attack */
      LIFTING,				/**< the hero is lifting an destroyable item (a pot, a bush, etc.) */
      TREASURE,				/**< the hero is brandishing a treasure */
      JUMPING,				/**< the hero is jumping */
      HURT,				/**< the hero is hurt */
      PLUNGING,				/**< the hero is plunging into water */
      FALLING,				/**< the hero is falling into a hole */
      RETURNING_TO_SOLID_GROUND,	/**< the hero is returning towards solid ground (e.g. after he drowned
					 * in deep water or falled into a hole) */
      STAIRS,				/**< the hero is being moved by stairs */
      VICTORY,				/**< the hero is make a victory sequence with his sword */
      USING_INVENTORY_ITEM,		/**< the hero is currently using an item from the inventory */
      FREEZED				/**< the hero cannot move for various possible reasons,
					 * including an instruction from the script */

      NB_STATES;
    };

  private:

    // state
    class State;

    State *states[];				/**< all possible states of the hero (each state has only one instance) */
    State *state;				/**< the current state */
    StateId state_id;				/**< id of current state of the hero (considered only when the game is not suspended) */

    // sprites
    Equipment *equipment;			/**< equipment of the player */
    HeroSprites *sprites;			/**< the hero's sprites (note that we don't use the sprites structure from MapEntity) */

    // position
    Detector *facing_entity;			/**< the entity just in front of the hero (or NULL) */
    static const int normal_walking_speed;	/**< default speed when walking */
    int walking_speed;				/**< current walking speed */
    Teletransporter *delayed_teletransporter;	/**< a teletransporter that will be activated when the hero finishes 
						 * a special behavior, such as falling into a hole or walking on stairs */

    // ground
    Ground ground;				/**< kind of ground under the hero: grass, shallow water, etc. */
    Rectangle last_solid_ground_coords;		/**< coordinates of the last hero position on a ground
						 * where he can walk (e.g. before jumping or falling into a hole) */
    Layer last_solid_ground_layer;		/**< layer of the last hero position on a solid ground */
    uint32_t next_ground_date;			/**< when something will happend with the ground next time (a sound or a movement) */

    // state
    void set_state(StateId state);

    // position
    void update_movement(void);
    void movement_just_changed(void);
    void try_snap_to_facing_entity(void);

    // ground
    void start_ground(void);
    void update_ground(void);

    // life
    void check_gameover();

  public:

    // creation and destruction
    Hero(Equipment *equipment);
    ~Hero(void);

    // features
    EntityType get_type(void);
    bool can_be_obstacle(void);
    bool can_detect_entities(void);
    bool can_be_displayed(void);
    bool is_displayed_in_y_order(void);

    // state
    StateId get_state(void);
    StateId get_state(StateId state);
    bool is_state(StateId state);

    // game loop
    void update(void);
    void display_on_map(void);
    void set_suspended(bool suspended);
    void key_pressed(GameControls::GameKey key);
    void key_released(GameControls::GameKey key);

    // sprites
    int get_animation_direction(void);
    void set_animation_direction(int direction);
    bool is_animation_finished(void);
    void rebuild_equipment(void);

    // map
    void set_map(Map *map);
    void set_map(Map *map, int initial_direction);
    void place_on_destination_point(Map *map);
    void opening_transition_finished(void);

    // position
    const Rectangle get_facing_point(void);
    const Rectangle get_facing_point(int direction4);
    Detector *get_facing_entity(void);
    void set_facing_entity(Detector *detector);
    bool is_facing_obstacle(void);
    bool is_facing_point_on_obstacle(void);
    bool is_on_raised_blocks(void);
    int get_walking_speed(void);
    void set_walking_speed(int walking_speed);

    // movement
    int get_wanted_movement_direction(void);
    int get_wanted_movement_direction8(void);
    int get_real_movement_direction(void);
    int get_real_movement_direction8(void);
    bool is_moving_towards(int direction4);
    bool is_direction_locked(void);
    void notify_position_changed(void);
    void notify_layer_changed(void);
    void notify_movement_changed(void);

    // ground
    Ground get_ground(void);
    void set_ground(Ground ground);
    bool is_ground_visible(void);

    // obstacles
    bool is_obstacle_for(MapEntity *other);
    bool is_water_obstacle(void);
    bool is_hole_obstacle(void);
    bool is_ladder_obstacle(void);
    bool is_block_obstacle(Block *block);
    bool is_teletransporter_obstacle(Teletransporter *teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt);
    bool is_stairs_obstacle(Stairs *stairs);
    bool is_sensor_obstacle(Sensor *sensor);
    bool is_raised_block_obstacle(CrystalSwitchBlock *raised_block);
    bool is_jump_sensor_obstacle(JumpSensor *jump_sensor);

    // collisions
    void notify_collision_with_enemy(Enemy *enemy);
    void notify_collision_with_enemy(Enemy *enemy, Sprite *enemy_sprite, Sprite *this_sprite);
    void notify_collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode);
    void notify_collision_with_conveyor_belt(ConveyorBelt *conveyor_belt, int dx, int dy);
    void notify_collision_with_stairs(Stairs *stairs, int collision_mode);
    void notify_collision_with_sensor(Sensor *sensor);
    void notify_collision_with_explosion(Explosion *explosion, Sprite *sprite_overlapping);

    void avoid_collision(MapEntity *entity, int direction);
    void try_snap_to_facing_entity(void);

    // enemies
    void just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed);
    bool is_striking_with_sword(Detector *detector);
    void hurt(MapEntity *source, int life_points, int magic_points);
    void get_back_from_death(void);

    // specific states
    void notify_grabbed_entity_collision(void);
    Stairs *get_stairs_overlapping(void);
    void give_treasure(Treasure *treasure);
    void jump(int direction8, int length, bool with_collisions, bool with_sound, Layer layer_after_jump = LAYER_NB, uint32_t movement_delay = 0);

};

#endif

