/**
 * This module defines the class HUD.
 */

#include "HUD.h"
#include "FileTools.h"

/**
 * Constructor.
 */
HUD::HUD(void) {
  img_hearts = FileTools::open_image("hud/hearts.png");
}

/**
 * Destructor.
 */
HUD::~HUD(void) {
  SDL_FreeSurface(img_hearts);
}

/**
 * Displays the HUD.
 * @param destination the destination surface
 */
void HUD::display(SDL_Surface *destination) {

  // hearts
  // rupees
  // magic bar
  // objects
  // icons
  
}
