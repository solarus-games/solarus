#ifndef ZSDX_HUD_H
#define ZSDX_HUD_H

#include "Common.h"

/**
 * This class displays information on the screen such as the rupees,
 * the hearts, etc.
 */
class HUD {

 private:

  Equipment *equipment;;

  int nb_elements;
  HudElement *elements[2];

 public:

  HUD(Game *game);
  ~HUD(void);

  void update();
  void display(SDL_Surface *destination);

};

#endif
