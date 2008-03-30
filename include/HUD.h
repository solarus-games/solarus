#ifndef ZSDX_HUD_H
#define ZSDX_HUD_H

#include "Common.h"
#include "HudElement.h"

/**
 * This class displays information on the screen such as the rupees,
 * the hearts, etc.
 */
class HUD {

 private:

  Savegame *savegame;

  int nb_elements;
  HudElement *elements[2];

 public:

  HUD(Savegame *savegame);
  ~HUD(void);

  void update();
  void display(SDL_Surface *destination);

};

#endif
