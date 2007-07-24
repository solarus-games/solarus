/**
 * This module defines the class LinkAnimations.
 */

#ifndef ZSDX_LINK_ANIMATIONS_H
#define ZSDX_LINK_ANIMATIONS_H

#include <SDL/SDL.h>
#include "SpriteAnimations.h"

/**
 * Link's animations.
 */
class LinkAnimations: public SpriteAnimations {

 private:

  /**
   * Only instance of LinkAnimations.
   */
  static LinkAnimations *instance;

  /**
   * Image from which animation stopped is extracted.
   */
  SDL_Surface *link_stopped_image;

  /**
   * Image from which animation walking is extracted.
   */
  SDL_Surface *link_walking_image;

  /**
   * Creates the animations. This function is called by
   * the constructor.
   */
  void create_animations(void);

  /**
   * Constructor.
   */
  LinkAnimations(void);

  /**
   * Destructor.
   */
  ~LinkAnimations(void);

 public:

  /**
   * Returns the only instance.
   */
  static LinkAnimations *get_instance(void);

  /**
   * Destroys the instance.
   */
  static void destroy_instance(void);
};

#endif
