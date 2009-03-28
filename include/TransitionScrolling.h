#ifndef ZSDX_TRANSITION_SCROLLING_H
#define ZSDX_TRANSITION_SCROLLING_H

#include "Common.h"
#include "Transition.h"

/**
 * Scrolling transition between two maps.
 */
class TransitionScrolling: public Transition {

 private:

  SDL_Surface *both_maps_surface;      /**< an intermediate surface where the two map surfaces will be blitted */
  int scrolling_direction;             /**< direction of the scrolling (0 to 3) */
  Uint32 next_scroll_date;             /**< date of the next scrolling step */

  int dx;                              /**< x change for each scrolling step */
  int dy;                              /**< y change for each scrolling step */

  static const SDL_Rect previous_map_dst_positions[];

  SDL_Rect previous_map_dst_position;  /**< position where the previous map is blitted on both_maps_surface */
  SDL_Rect current_map_dst_position;   /**< position where the current map is blitted on both_maps_surface */
  SDL_Rect current_scrolling_position; /**< the rectangle from both_maps_surface that is currently visible */

  void scroll(void);

 public:
  TransitionScrolling(Direction direction);
  ~TransitionScrolling(void);

  bool needs_previous_surface(void);

  void start(void);
  bool is_started(void);
  bool is_over(void);
  void display(SDL_Surface *surface);
};

#endif
