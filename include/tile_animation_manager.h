#ifndef ZSDX_TILE_ANIMATION_MANAGER_H
#define ZSDX_TILE_ANIMATION_MANAGER_H

#include <SDL/SDL.h>

/* Tile animation sequence type: 1-2-3-2 or 1-2-3
 */
enum tile_animation_sequence_t {
  ANIMATION_SEQUENCE_1232,
  ANIMATION_SEQUENCE_123,
};

/* Function called by the SDL timer every 250 ms
 * to change the animation frame
 */
Uint32 tile_animation_next_frame(Uint32 interval, void *param);

/* This class updates during the game the animation frame
 * of the animated tiles.
 * Two kinds of animation sequences are handled: 1-2-3-2 and 1-2-3.
 */
class TileAnimationManager {

  /* Function called by the SDL timer every 250 ms
   * to change the animation frame
   */
  friend Uint32 tile_animation_next_frame(Uint32 interval, void *param);

 private:
  /* Frame counter (0 to 11), increased every 250 ms
   */
  short frame_counter;

  /* Current frame for both sequences
   */
  short current_frames[2];

  /* Increment the frame counter and updates current_frame.
   */
  void increment_frame_counter(void);

 public:
  /* Constructor
   */
  TileAnimationManager(void);

  /* Destructor
   */
  inline ~TileAnimationManager(void) { }

  /* Return the current_frame (0, 1 or 2) of a sequence
   * 1-2-3-2 or 1-2-3
   */
  inline short get_current_frame(tile_animation_sequence_t sequence) { return current_frames[sequence]; }
};
#endif
