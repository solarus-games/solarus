/**
 * This module defines the class HUD.
 */

#include "HUD.h"
#include "FileTools.h"
#include "Savegame.h"
#include "HeartsView.h"

/**
 * Constructor.
 */
HUD::HUD(Savegame *savegame):
  savegame(savegame) {

  hearts_view = new HeartsView(savegame, 216, 14);
}

/**
 * Destructor.
 */
HUD::~HUD(void) {
  delete hearts_view;
}

/**
 * Update the HUD.
 */
void HUD::update(void) {
  hearts_view->update();
}

/**
 * Displays the HUD.
 * @param destination the destination surface
 */
void HUD::display(SDL_Surface *destination) {

  // hearts
  hearts_view->display(destination);

  // rupees
  // magic bar
  // objects
  // icons
  
}
