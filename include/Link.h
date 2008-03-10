/**
 * This module defines the class Link.
 */

#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include "Common.h"
#include "Moving8ByPlayer.h"

/**
 * Link's entity.
 * It is animated and can be controlled with an 8 directions system.
 */
class Link: public Moving8ByPlayer {

 protected:

  /**
   * Link's sprite.
   */
  AnimatedSprite *sprite;

  /**
   * String constants corresponding to Link sprite's description.
   */
  static const SpriteId link_sprite_ids[3];

  /**
   * Redefinition of Moving8ByPlayer::update_movement
   * to take care of link's animation.
   * This function is called when the user presses or
   * releases a keyboard arrow.
   */
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
};

#endif
