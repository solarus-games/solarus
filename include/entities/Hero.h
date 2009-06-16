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
#include "entities/Enemy.h"
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
    USING_INVENTORY_ITEM,        /**< the hero is currently using an item from the inventory */
    FREEZED,                     /**< the hero cannot move for various possible reasons,
				  * including an instruction from the script */
  };

 private:

  // equipment
  Equipment *equipment;          /**< equipment of the player */

  Sprite *tunic_sprite;          /**< sprite of the current tunic */
  Sprite *sword_sprite;          /**< current sword sprite */
  Sprite *sword_stars_sprite;    /**< stars running along the sword when the sword is loading */
  Sprite *shield_sprite;         /**< current shield sprite */
  Sprite *shadow_sprite;         /**< shadow of the hero, only in state JUMPING (in other states,
			          * the shadow is with the tunic sprite) */
  Sprite *ground_sprite;         /**< ground displayed under the hero (e.g. grass or shallow water) */

  Sound *sword_sound;            /**< sound of the current sword */
  Sound *ground_sound;           /**< sound of the current ground displayed under the hero */

  static const SpriteAnimationSetId tunic_sprite_ids[];       /**< name of each tunic sprite */
  static const SpriteAnimationSetId sword_sprite_ids[];       /**< name of each sword sprite */
  static const SpriteAnimationSetId sword_stars_sprite_ids[]; /**< name of each sword stars sprite */
  static const SpriteAnimationSetId shield_sprite_ids[];      /**< name of each shield sprite */
  static const SpriteAnimationSetId ground_sprite_ids[];      /**< name of each ground sprite */
  static const SoundId sword_sound_ids[];                     /**< name of each sword sound */
  static const SoundId ground_sound_ids[];                    /**< name of each ground sound */

  // movement
  PlayerMovement *normal_movement;
  static const int walking_speed;                             /**< speed of the player movement */
  static const int animation_directions[];                    /**< direction of the animation for each arrow key combination */
  bool move_tried;                                            /**< indicates that the hero just tried to move */
  int old_x;                                                  /**< x position of the hero before the last move try */
  int old_y;                                                  /**< y position of the hero before the last move try */

  // state
  State state;                   /**< current state of the hero (considered only when the game is not suspended) */
  Detector *facing_entity;       /**< the entity just in front of the hero */
  int animation_direction_saved; /**< direction of the hero's sprites, saved before
				  * showing a sprite animation having only one direction */
  Uint32 end_blink_date;         /**< when the hero's sprites stop blinking */

  int counter;                   /**< counter incremented every 100 ms in certain conditions:
				  * - in state FREE: counts for how long the hero is trying to walk
				  * to a wall (animation pushing is triggered at 800 ms)
				  * - in state SWORD_LOADING: counts for how long the hero is loading
				  * his sword (the spin attack is possible after 1000 ms) */

  Uint32 next_counter_date;      /**< when the counter will be incremented */

  // pushing
  Uint16 pushing_direction_mask; /**< direction of the hero's movement when pushing
				  * (0xFFFF indicates that he is currently not trying to push) */
  Detector *grabbed_entity;      /**< the entity the hero is pushing or pulling */

  // walking
  bool walking;                  /**< stopped or walking? (used in states FREE, PUSHING and CARRYING) */

  // sword loading
  bool sword_loaded;             /**< in state SWORD_LOADING, becomes true when the spin attack is possible */
  Uint32 next_hit_sound_date;    /**< when the sword hit sound has to be played next time */

  // lift and carry an object
  CarriedItem *lifted_item;      /**< item being lifted or carried */
  CarriedItem *thrown_item;      /**< item thrown and not broken yet */

  // brandish a treasure
  Treasure *treasure;            /**< the treasure being brandished (if any) */

  // jump
  int jump_y;                    /**< height of the hero's sprite when jumping, relative to its shadow on the ground */
  Layer layer_after_jump;        /**< the layer to set when the jump movement is finished */

  // return to solid ground
  SDL_Rect last_solid_ground_coords;     /**< coordinates of the last hero position on a ground
				          * where he can walk (e.g. before jumping or falling into a hole) */
  SDL_Rect target_solid_ground_coords;   /**< coordinates of the position where the hero will go if he falls
					  * into a hole (or some other bad ground), or (-1,-1) to indicate
					  * that the hero will just return to the last solid ground coordinates */

  // special ground under the hero
  Ground ground;                         /**< kind of ground under the hero: grass, shallow water, etc. */
  Uint32 next_ground_sound_date;         /**< when the ground sound has to be played next time */

  // items
  InventoryItem *current_inventory_item; /**< the inventory item the player is currently using,
					  * or NULL if he is not using an item */
  Uint32 when_can_use_inventory_item;    /**< date when the player can use an inventory item next time */

  // update functions
  void update_position(void);
  void update_sprites(void);
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
  bool is_ground_visible(void);

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
  void start_falling(void);
  void update_falling(void);

  void start_returning_to_solid_ground(const SDL_Rect &target);
  void update_returning_to_solid_ground(void);

  // animation of the sprites
  void set_animation_direction(int direction);
  void set_animation_direction_from_movement(void);
  bool is_direction_locked(void);
  void stop_displaying_sword(void);
  void blink(void);
  void stop_blinking(void);

  void set_animation_sword(void);
  void set_animation_sword_tapping(void);
  void set_animation_walking(void);
  void set_animation_grabbing(void);
  void set_animation_pulling(void);
  void set_animation_pushing(void);
  void set_animation_lifting(void);
  void set_animation_jumping(void);
  void set_animation_hurt(void);
  void set_animation_plunging(void);
  void set_animation_falling(void);

  void save_animation_direction(void);
  void restore_animation_direction(void);

  bool is_visible(void);
  bool is_sword_visible(void);
  bool is_sword_stars_visible(void);
  bool is_shield_visible(void);
  bool is_shadow_visible(void);

 public:

  // creation and destruction
  Hero(Equipment *equipment);
  ~Hero(void);

  // properties
  EntityType get_type(void);

  void update(void);
  void display_on_map(void);

  // movement
  PlayerMovement * get_normal_movement(void);
  int get_movement_direction(void);
  bool is_moving_towards(int direction);
  const SDL_Rect get_facing_point(void);
  const SDL_Rect get_facing_point(int direction);
  Detector *get_facing_entity(void);
  void just_moved(void);
  void set_facing_entity(Detector *detector);
  bool is_facing_obstacle(void);

  // map
  void set_map(Map *map);
  void set_map(Map *map, int initial_direction);
  void place_on_destination_point(Map *map);
  void opening_transition_finished(void);

  // sprites
  int get_animation_direction(void);
  void set_suspended(bool suspended);
  void rebuild_equipment(void);
  void restart_animation(void);
  void set_animation_stopped(void);

  // state of the hero
  State get_state(void);
  void set_ground(Ground ground);
  void start_free(void);
  void start_sword(void);
  bool is_grabbing_or_pulling(void);
  bool is_moving_grabbed_entity(void);
  void grabbed_entity_collision(void);
  void stop_moving_grabbed_entity(void);
  void start_lifting(DestructibleItem *item_to_lift);
  void start_carrying(void);
  void freeze(void);
  void unfreeze(void);
  void give_treasure(Treasure *treasure);
  void start_jumping(int direction, int length, bool with_collisions);
  void start_jumping(int direction, int length, bool with_collisions, Layer layer_after_jump);
  void hurt(MapEntity *source, int life);
  void get_back_from_death(void);
  void set_target_solid_ground_coords(const SDL_Rect &target_solid_ground_coords);

  // keys
  void key_pressed(Controls::GameKey key);
  void key_released(Controls::GameKey key);

  // enemies and collisions
  bool is_obstacle_for(MapEntity *other);
  virtual void collision_with_enemy(Enemy *enemy);
  virtual void collision_with_enemy(Enemy *enemy, Sprite *sprite_overlapping);
  void just_attacked_enemy(Enemy::Attack attack, Enemy *victim);
  bool is_stroke_by_sword(Detector *detector);

  virtual void collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode);
  bool is_water_obstacle(void);
  bool is_hole_obstacle(void);
  bool is_teletransporter_obstacle(Teletransporter *teletransporter);
  bool is_sensor_obstacle(Sensor *sensor);
};

#endif
