/**
 * This module defines the class HUD.
 */

#include "HUD.h"
#include "Game.h"
#include "Savegame.h"
#include "HeartsView.h"
#include "RupeesView.h"
#include "ActionIcon.h"
#include "SwordIcon.h"
#include "PauseIcon.h"

/**
 * Constructor.
 */
HUD::HUD(Game *game) {

  Equipment *equipment = game->get_savegame()->get_equipment();
  KeysEffect *keys_effect = game->get_keys_effect();

  nb_elements = 0;

  elements[nb_elements++] = new HeartsView(equipment, 216, 14);
  elements[nb_elements++] = new RupeesView(equipment, 8, 216);
  //  elements[nb_elements++] = new ActionIcon(keys_effect, 23, 51);
  elements[nb_elements++] = new SwordIcon(keys_effect, equipment, 10, 29);
  elements[nb_elements++] = new PauseIcon(keys_effect, -3, 7);
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
