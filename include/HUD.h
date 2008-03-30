/**
 * This module defines the class HUD.
 */

#ifndef ZSDX_HUD_H
#define ZSDX_HUD_H

#include "Common.h"

/**
 * This class displays information on the screen such as the rupees,
 * the hearts, etc.
 */
class HUD {

 private:

  // all images
  SDL_Surface *img_hearts;

 public:

  HUD(void);
  ~HUD(void);

  void display(SDL_Surface *destination);

};

#endif
