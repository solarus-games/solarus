/**
 * This module defines the class HUD.
 */

#include "HUD.h"
#include "FileTools.h"
#include "Savegame.h"
#include "HeartsView.h"
#include "RupeesView.h"

/**
 * Constructor.
 */
HUD::HUD(Savegame *savegame):
  savegame(savegame) {

  nb_elements = 0;

  elements[nb_elements++] = new HeartsView(savegame, 216, 14);
  elements[nb_elements++] = new RupeesView(savegame, 8, 216);
}

/**
 * Destructor.
 */
HUD::~HUD(void) {

  for (int i = 0; i < nb_elements; i++) {
    delete elements[i];
  }
}

/**
 * Updates the information displayed by HUD.
 */
void HUD::update(void) {

  for (int i = 0; i < nb_elements; i++) {
    elements[i]->update();
  }
}

/**
 * Displays the HUD.
 * @param destination the destination surface
 */
void HUD::display(SDL_Surface *destination) {

  for (int i = 0; i < nb_elements; i++) {
    elements[i]->display(destination);
  }
}
