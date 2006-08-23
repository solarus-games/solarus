#ifndef ZSDX_ANIMATED_H
#define ZSDX_ANIMATED_H

#include "map_object.h"
#include "sprite_animation.h"

/* Abstract class for the animated sprites.
 * An animated sprite can have several animations.
 * Each animation correspond to a specific situation
 * of the sprite.
 *
 * For example, you can have an animation "Stopped"
 * and an animation "Walking".
 *
 * A subclass of AnimatedSprite has to create its
 * animations and AnimatedSprite takes care of
 * the display.
 */
class AnimatedSprite: virtual public MapObject {

 protected:
  /* Number of animations
   */
  const int nb_animations;

  /* Current animation (first is number 0)
   */
  int current_animation;

  /* Array of SpriteAnimation* containing the animations
   * This array is allocated in the constructor and
   * destroyed in the destructor. It is filled in the
   * abstract function create_animations.
   */
  SpriteAnimation **animations;

  /* Constructor
   */
  AnimatedSprite(int nb_animations);

  /* Destructor
   */
  virtual ~AnimatedSprite(void);

  /* Fill the array of animations
   * This function should be called by the constructor
   * of the subclass which implements it.
   */
  virtual void create_animations(void) = 0;

 public:
  /* Display the sprite on the map
   * This function just calls display_on_map() on the current
   * animation.
   */
  void display_on_map(SDL_Surface *map);
};

#endif
