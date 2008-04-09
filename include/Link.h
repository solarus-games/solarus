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
 */
enum LinkState {
  LINK_STATE_FREE,                    /**< normal state */
  LINK_STATE_CARRYING,                /**< Link can walk but he is carrying a pot or a bush */
  LINK_STATE_SWORD_LOADING,           /**< Link can walk but his sword is loading for a spin attack */
  LINK_STATE_PUSHING_OR_PULLING,      /**< Link is pushing or pulling an object */
  LINK_STATE_SWIMMING,                /**< Link is swiming */
  LINK_STATE_SWORD_SWINGING,          /**< Link is swinging his sword or releasing a spin attack */
  LINK_STATE_NO_CONTROL,              /**< Link cannot move */
};
// TODO check this in start_left, start_right etc
// modify in set_control_enabled

/**
 * Link's entity.
 * It is animated and can be controlled with an 8 directions system.
 */
class Link: public Moving8ByPlayer, AnimationListener {

 private:

  /**
   * Current state of Link.
   */
  LinkState state;

  /**
   * Equipment of the player.
   */
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

  void update_movement(void);
  void set_animation_direction(int direction);
  void set_animation_stopped(void);
  void set_animation_walking(void);
  void set_animation_sword(void);

 public:

  Link(void);
  ~Link(void);

  void initialize_sprites(void);
  void update_sprites(void);

  void set_map(Map *map, int initial_direction);

  void update(void);
  void display_on_map(Map *map);

  LinkState get_state(void);
  void set_state(LinkState state);

  void start_sword(void);
  bool is_sword_visible(void);
  bool is_shield_visible(void);

  void animation_over(AnimatedSprite *sprite);
};

#endif
