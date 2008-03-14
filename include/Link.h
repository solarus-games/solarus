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
   * Link's sprite.
   */
  AnimatedSprite *sprite;

  /**
   * String constants corresponding to Link sprite's description.
   */
  static const SpriteId link_sprite_ids[3];

  void update_movement(void);

 public:

  Link(void);
  ~Link(void);

  /**
   * Returns Link's sprite.
   * @return Link's sprite
   */
  inline AnimatedSprite *get_sprite(void) {
    return sprite;
  }

  void set_sprite(void);

  void set_map(Map *map);
  void display_on_map(Map *map);

  LinkState get_state(void);
  void set_state(LinkState state);

  void start_sword(void);

  void animation_over(AnimatedSprite *sprite);
};

#endif
