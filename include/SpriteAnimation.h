/**
 * This module defines the class SpriteAnimation.
 */

#ifndef ZSDX_SPRITE_ANIMATION_H
#define ZSDX_SPRITE_ANIMATION_H

#include "Common.h"
#include "SpriteAnimationDirection.h"
#include "Map.h"

/**
 * A sprite animation is a list of animated sequences.
 * Each animated sequence correspond to a direction
 * of the sprite.
 */
class SpriteAnimation {

 private:

  /**
   * Number of directions of the animation.
   */
  const int nb_directions;

  /**
   * Array of directions.
   * Each direction is a sequence of images.
   */
  SpriteAnimationDirection **directions;

  /**
   * X coordinate of the sprite's origin from the
   * upper-left corner of its image.
   */
  const int x_hotspot;

  /**
   * Y coordinate of the sprite's origin from the
   * upper-left corner of its image.
   */
  const int y_hotspot;

  /**
   * Interval in milliseconds between two frames.
   * This interval is the same for all directions.
   * If there is only one frame in every direction, this field is set to zero.
   */
  const Uint32 frame_interval;
  
 public:

  /**
   * Constructor.
   * @param nb_directions number of directions in this animation
   * @param directions the image sequence of each direction
   * @param x_hotspot x coordinate of the sprite's origin
   * @param y_hotspot y coordinate of the sprite's origin
   * @param frame_interval interval in millisecond between two frames for this sprite animation
   */
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, int x_hotspot, int y_hotspot, Uint32 frame_interval);

  /**
   * Simple constructor without specifying frame interval.
   * You can use this constructor if your directions contains only one frame.
   * @param nb_directions number of directions in this animation
   * @param directions the image sequence of each direction
   * @param x_hotspot x coordinate of the sprite's origin
   * @param y_hotspot y coordinate of the sprite's origin
   */
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, int x_hotspot, int y_hotspot);

  /**
   * Destructor.
   */
  virtual ~SpriteAnimation(void);

  /**
   * Returns a direction.
   * @param direction the direction
   * @return the sequence of images corresponding to this direction
   */
  inline SpriteAnimationDirection *get_direction(int direction) { return directions[direction]; }

  /**
   * Returns the interval between two frames for this sprite animation.
   * @return the frame interval in millisecond
   */
  inline Uint32 get_frame_interval(void) { return frame_interval; }

  /**
   * Displays a specific frame of this animation on the map.
   * @param map the map
   * @param position_in_map position of the sprite on the map
   * @param current_direction the direction to show
   * @param current_frame the frame to show in this direction
   */
  void display_on_map(Map *map, SDL_Rect &position_in_map, int current_direction, int current_frame);
};

#endif
