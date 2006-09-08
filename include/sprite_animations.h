#ifndef ZSDX_SPRITE_ANIMATIONS_H
#define ZSDX_SPRITE_ANIMATIONS_H

#include <SDL/SDL.h>
#include "sprite_animation.h"
#include "map.h"

/* Class for a set of animations
 * Each animation correspond to a specific situation
 * of a sprite.
 *
 * For example, you can have an animation "Stopped"
 * and an animation "Walking".
 */
class SpriteAnimations {

 protected:
  /* Number of animations
   */
  const int nb_animations;

  /* Array of SpriteAnimation* containing the animations
   * This array is allocated in the constructor and
   * destroyed in the destructor. It is filled in the
   * abstract function create_animations.
   */
  SpriteAnimation **animations;

  /* Constructor
   */
  SpriteAnimations(int nb_animations);

  /* Destructor
   */
  virtual ~SpriteAnimations(void);

  /* Create the animations
   * This function should be called by the constructor.
   */
  virtual void create_animations(void) = 0;

 public:
  /* Return an animation
   */
  inline SpriteAnimation *get_animation(int animation) { return animations[animation]; }

  /* Display the given frame of the animation on the map
   * This function just calls display_on_map() on the current
   * animation.
   */
  void display_on_map(Map *map, SDL_Rect &where_in_map,
		      int current_animation, int current_direction, int current_frame);
};


#endif
