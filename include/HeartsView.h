#ifndef ZSDX_HEARTS_VIEW_H
#define ZSDX_HEARTS_VIEW_H

#include "Common.h"
#include "HudElement.h"

/**
 * This class handles the player's hearts displaying.
 */
class HeartsView: public HudElement {

 private:
  
  // rectangles in the image
  static SDL_Rect empty_heart_position;
  static SDL_Rect full_heart_position;
  static SDL_Rect fraction_heart_positions[3];

  SDL_Surface *img_hearts;
  int nb_max_hearts_displayed;
  int nb_current_hearts_displayed;
  
  Uint32 next_heart_update_date;
  Uint32 next_danger_sound_date;
  
 public:

  HeartsView(Savegame *savegame, int x, int y);
  ~HeartsView(void);

  void update(void);
  void rebuild(void);
};

#endif
