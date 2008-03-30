#ifndef ZSDX_HUD_H
#define ZSDX_HUD_H

#include "Common.h"

/**
 * This class displays information on the screen such as the rupees,
 * the hearts, etc.
 */
class HUD {

 private:

  Savegame *savegame;

  // hearts
  HeartsView *hearts_view;

 public:

  HUD(Savegame *savegame);
  ~HUD(void);

  void update();
  void display(SDL_Surface *destination);

};

#endif
