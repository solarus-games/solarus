#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include "Common.h"
#include "MapEntity.h"
#include "AnimationListener.h"

/**
 * Possible states of Link.
 * The state is considered only when the game is not suspended.
 * Link can move in states LINK_STATE_FREE, LINK_STATE_PUSHING,
 * LINK_STATE_CARRYING, LINK_STATE_SWORD_LOADING and LINK_STATE_SWIMMING.
 * Link can swing his sword in states LINK_STATE_FREE, LINK_STATE_PUSHING,
 * LINK_STATE_CARRYING and LINK_STATE_SWORD_SWINGING.
 */
enum LinkState {
  LINK_STATE_FREE,                    /**< normal state (stopped or walking) */
  LINK_STATE_PUSHING,                 /**< Link is trying to push an obstacle */
  LINK_STATE_CARRYING,                /**< Link can walk but he is carrying a pot or a bush */
  LINK_STATE_SWORD_LOADING,           /**< Link can walk but his sword is loading for a spin attack */
  LINK_STATE_SWIMMING,                /**< Link is swimming */
  LINK_STATE_GRABBING,                /**< Link is grabbing an object and can push or pull it */
  LINK_STATE_SWORD_SWINGING,          /**< Link is swinging his sword */
  LINK_STATE_SPIN_ATTACK,             /**< Link is releasing a spin attack */
  LINK_STATE_LIFTING,                 /**< Link is lifting an transportable item (a pot, a bush, etc.) */
};

/**
 * Link's entity.
 * It is animated and can be controlled with an 8 directions system.
 * This class handles Link's actions: the movements and the animation of his sprites.
 */
class Link: public MapEntity, AnimationListener {

 private:

  // the map
  Map *map;

  // equipment of the player
  Equipment *equipment;

  // Link's sprites
  Sprite *tunic_sprite;
  Sprite *sword_sprite;
  Sprite *sword_stars_sprite;
  Sprite *shield_sprite;
  Sound *sword_sound;

  static const SpriteAnimationsId tunic_sprite_ids[];
  static const SpriteAnimationsId sword_sprite_ids[];
  static const SpriteAnimationsId sword_stars_sprite_ids[];
  static const SpriteAnimationsId shield_sprite_ids[];
  static const SoundId sword_sound_ids[];

  /**
   * Current state of Link.
   * The state is considered only when the game is not suspended.
   */
  LinkState state;
  bool walking; // stopped or walking? (used in states free, pushing and carrying)

  /**
   * Counter incremented every 100 ms in certain conditions.
   * - In state LINK_STATE_FREE: counts for how long Link is trying to walk
   * to a wall (animation pushing is triggered at 800 ms)
   * - In state LINK_STATE_SWORD_LOADING: counts for how long Link is loading
   * his sword (the spin attack is possible after 1000 ms)
   */
  int counter;
  Uint32 next_counter_date;

  // pushing
  Uint16 pushing_direction_mask; // direction of Link's movement when pushing
                                 // 0xFFFF indicates that he is not trying to push

  // spin attack
  bool sword_loaded; // in state LINK_STATE_SWORD_LOADING, becomes true when the spin attack is possible

  // lift and carry an object
  MapEntity *facing_entity;
  CarriedItem *carried_item;

  // update functions
  void update_position(void);
  void update_sprites(void);

  void movement_just_changed(void);

  // Link's state
  void set_state(LinkState state);
  void start_sword_loading(void);
  void update_sword_loading(void);
  void start_spin_attack(void);
  bool can_start_sword(void);
  void start_lifting(void);
  void start_throwing(void);

  // animation of the sprites
  void set_animation_direction(int direction);
  bool is_direction_locked(void);
  void stop_displaying_sword(void);

  void set_animation_sword(void);
  void set_animation_walking(void);
  void set_animation_pushing(void);
  void set_animation_lifting(void);

 public:

  // creation and destruction
  Link(void);
  ~Link(void);

  // movement
  Movement8ByPlayer *get_movement(void);
  SDL_Rect get_facing_point(void);
  void just_moved(void);
  void set_facing_entity(EntityDetector *detector);

  // map
  void set_map(Map *map, int initial_direction);
  void update(void);
  void display_on_map(Map *map);

  // sprites
  int get_animation_direction(void);
  void set_suspended(bool suspended);

  void initialize_sprites(void);
  bool is_sword_visible(void);
  bool is_sword_stars_visible(void);
  bool is_shield_visible(void);

  void restart_animation(void);
  void animation_over(Sprite *sprite);

  void set_animation_stopped(void);

  // state of Link
  LinkState get_state(void);
  void start_free(void);
  void start_sword(void);
  void start_pushing(void);
  void start_carrying(void);
  void action_key_pressed(void);
};

#endif
