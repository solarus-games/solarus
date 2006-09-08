#ifndef ZSDX_LINK_ANIMATIONS_H
#define ZSDX_LINK_ANIMATIONS_H

#include <SDL/SDL.h>
#include "sprite_animations.h"

/* Link's animations
 */
class LinkAnimations: public SpriteAnimations {

 private:
  /* Only instance
   */
  static LinkAnimations *instance;

  /* Image from which link's stopped animation is extracted
   */
  SDL_Surface *link_stopped_image;

  /* Image from which link's walking animation is extracted
   */
  SDL_Surface *link_walking_image;

  void create_animations(void);

  /* Constructor
   */
  LinkAnimations(void);

  /* Destructor
   */
  ~LinkAnimations(void);

 public:
  /* Return the only instance
   */
  static LinkAnimations *get_instance(void);

  /* Destroy the instance
   */
  static void destroy_instance(void);
};

#endif
