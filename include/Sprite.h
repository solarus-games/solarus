#ifndef ZSDX_SPRITE_H
#define ZSDX_SPRITE_H

#include "Common.h"

/**
 * Represents an animated sprite.
 * An animated sprite is represented as a set of animations (an
 * instance of SpriteAnimationSet), a current animation,
 * a current direction and a current frame.
 * Several sprites can have the same animation set (i.e. they share
 * the same SpriteAnimationSet object).
 *
 * A sprite can be displayed directly on a surface, or it can
 * be attached to a map entity, allowing it to have a position on
 * the map and a movement.
 */
class Sprite {

 private:

  // animation set
  SpriteAnimationSetId animation_set_id; /**< id of this sprite's animation set */
  SpriteAnimationSet *animation_set;     /**< animation set of this sprite */

  // current state of the sprite

  string current_animation_name;         /**< name of the current animation */
  SpriteAnimation *current_animation;    /**< the current animation */
  int current_direction;                 /**< current direction of the animation (the first one is number 0);
				          * it can be different from the movement direction
				          * of the entity, because sometimes a sprite can
				          * go backwards. */
  int current_frame;                     /**< current frame of the animation (the first one is number 0) */
  bool frame_changed;                    /**< indicates that the frame has just changed */

  Uint32 next_frame_date;                /**< date of the next frame */

  bool suspended;                        /**< true if the animation is suspended */
  bool finished;                         /**< true if the animation has been stopped because the last frame is finished */

  // blink

  Uint32 blink_delay;                    /**< blink delay of the sprite, or zero if the sprite is not blinking */
  bool blink_is_sprite_visible;          /**< when blinking, true if the sprite is visible or false if it is invisible */
  Uint32 blink_next_change_date;         /**< date of the next change when blinking: visible or not */

  Uint32 get_frame_interval(void);
  int get_next_frame(void);

 public:

  // creation and destruction
  Sprite(SpriteAnimationSetId id);
  ~Sprite(void);

  SpriteAnimationSetId get_animation_set_id(void);
  SpriteAnimationSet *get_animation_set(void);

  // size and origin point
  SDL_Rect& get_size(void);
  SDL_Rect& get_origin(void);

  // current animation, direction and frame
  string get_current_animation(void);
  void set_current_animation(string animation_name);
  int get_current_direction(void);
  void set_current_direction(int current_direction);
  int get_current_frame(void);
  void set_current_frame(int current_frame);

  // animation state
  bool is_animation_started(void);
  void start_animation(void);
  void restart_animation(void);
  void stop_animation(void);

  bool is_suspended(void);
  void set_suspended(bool suspended);
  bool is_animation_finished(void);
  bool is_last_frame_reached(void);
  bool has_frame_changed(void);

  // blink
  bool is_blinking(void);
  void set_blinking(Uint32 blink_delay);

  // collisions
  bool check_collision(Sprite *other, int x1, int y1, int x2, int y2);

  // udpate and display
  void update(void);
  void display(SDL_Surface *destination, int x, int y);
};

#endif
