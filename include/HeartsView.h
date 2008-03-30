#ifndef ZSDX_HEARTS_VIEW_H
#define ZSDX_HEARTS_VIEW_H

#include "Common.h"

/**
 * This class handles the player's hearts displaying.
 */
class HeartsView {

 private:
  
  // rectangles in the image
  static SDL_Rect empty_heart_position;
  static SDL_Rect full_heart_position;
  static SDL_Rect fraction_heart_positions[3];

  Savegame *savegame;

  /**
   * The surface drawn.
   */
  SDL_Surface *hearts_surface;

  /**
   * Position of the hearts on the destination surface.
   */
  SDL_Rect destination_position;

  Uint32 next_heart_update_date;
  
 public:

  HeartsView(Savegame *savegame, int x, int y);
  ~HeartsView(void);

  SDL_Surface *img_hearts;
  int nb_max_hearts_displayed;
  int nb_current_hearts_displayed;
  
  void update(void);
  void redraw(void);
  void display(SDL_Surface *destination);
};

#endif
