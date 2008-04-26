/**
 * This module defines the class Link.
 */

#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include "Common.h"
#include "Moving8ByPlayer.h"
#include "AnimationListener.h"

/**
 * Possible states of Link.
 * The state is considered only when the game is not suspended.
 * Link can move in states LINK_STATE_FREE, LINK_STATE_PUSHING,
 * LINK_STATE_CARRYING, LINK_STATE_SWORD_LOADING and LINK_STATE_SWIMMING.
 * Link can swing his sword in all states except LINK_STATE_SWORD_LOADING,
 * LINK_STATE_SWIMMING and LINK_STATE_SPIN_ATTACK.
 */
enum LinkState {
  LINK_STATE_FREE,                    /**< normal state */
  LINK_STATE_PUSHING,                 /**< Link is trying to push an obstacle */
  LINK_STATE_CARRYING,                /**< Link can walk but he is carrying a pot or a bush */
  LINK_STATE_SWORD_LOADING,           /**< Link can walk but his sword is loading for a spin attack */
  LINK_STATE_SWIMMING,                /**< Link is swimming */
  LINK_STATE_GRABBING,                /**< Link is grabbing an object and can push or pull it */
  LINK_STATE_SWORD_SWINGING,          /**< Link is swinging his sword */
  LINK_STATE_SPIN_ATTACK,             /**< Link is releasing a spin attack */
};

/**
 * Link's entity.
 * It is animated and can be controlled with an 8 directions system.
 */
class Link: public Moving8ByPlayer, AnimationListener {

 private:

  // equipment of the player
  Equipment *equipment;

  // Link's sprites
  AnimatedSprite *tunic_sprite;
  AnimatedSprite *sword_sprite;
  AnimatedSprite *shield_sprite;
  Sound *sword_sound;

  static const SpriteId tunic_sprite_ids[];
  static const SpriteId sword_sprite_ids[];
  static const SpriteId shield_sprite_ids[];
  static const SoundId sword_sound_ids[];

  /**
   * Current state of Link.
   * The state is considered only when the game is not suspended.
   */
  LinkState state;
  bool walking; // stopped or walking?

  // pushing
  int pushing_counter;
  Uint32 next_pushing_counter_date;
  Uint16 pushing_direction_mask;

  // update functions
  void update_movement(void);
  void update_position(void);
  void update_sprites(void);

  // Link's state
  void set_state(LinkState state);

  // animation of the sprites
  void set_animation_direction(int direction);

  void set_animation_sword(void);
  void set_animation_stopped(void);
  void set_animation_walking(void);
  void set_animation_pushing(void);

 public:

  // creation and destruction
  Link(void);
  ~Link(void);

  // map
  void set_map(Map *map, int initial_direction);
  void update(void);
  void display_on_map(Map *map);

  // sprites
  void initialize_sprites(void);
  bool is_sword_visible(void);
  bool is_shield_visible(void);

  void set_animation_suspended(bool suspended);
  void restart_animation(void);
  void animation_over(AnimatedSprite *sprite);

  // state of Link
  LinkState get_state(void);
  void start_free(void);
  void start_sword(void);
  void start_pushing(void);
};

#endif
