#ifndef ZSDX_SPRITE_ANIMATION_H
#define ZSDX_SPRITE_ANIMATION_H

#include <SDL/SDL.h>
#include "sprite_animation_direction.h"
#include "map.h"

/* An animation is a list of animated sequences.
 * Each animated sequence correspond to a direction
 * of the sprite.
 */
class SpriteAnimation {

 private:
  /* Number of directions of the animation
   */
  const int nb_directions;

  /* Array of directions
   * Each direction is a sequence of images.
   */
  SpriteAnimationDirection **directions;

  /* X coordinate of the origin
   */
  const int x_hotspot;

  /* Y coordinate of the origin
   */
  const int y_hotspot;

  /* Interval in milliseconds between two frames
   * This interval is the same for all directions.
   * If there is only one frame in every directions, this field is set to zero.
   */
  const Uint32 frame_interval;
  
 public:
  /* Constructor
   */
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, int x_hotspot, int y_hotspot, Uint32 frame_interval);

  /* Simple constructor without frame_interval
   * You can use this constructor if your directions contains only one frame.
   */
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, int x_hotspot, int y_hotspot);

  /* Destructor
   */
  virtual ~SpriteAnimation(void);

  /* Return a direction
   */
  inline SpriteAnimationDirection *get_direction(int direction) { return directions[direction]; }

  /* Return the frame interval
   */
  inline Uint32 get_frame_interval(void) { return frame_interval; }

  /* Display the current frame of the current direction on the map
   */
  void display_on_map(Map *map, SDL_Rect &where_in_map, int current_direction, int current_frame);
};

#endif
