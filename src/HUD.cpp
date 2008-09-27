#include "HUD.h"
#include "Game.h"
#include "HeartsView.h"
#include "RupeesView.h"
#include "MagicBar.h"
#include "ActionIcon.h"
#include "SwordIcon.h"
#include "PauseIcon.h"
#include "ItemIcon.h"
#include "SmallKeysCounter.h"

/**
 * Constructor.
 */
HUD::HUD(Game *game):
  game(game), nb_elements(0), showing_message(false) {

  Savegame *savegame = game->get_savegame();
  Equipment *equipment = game->get_equipment();
  KeysEffect *keys_effect = game->get_keys_effect();

  elements[nb_elements++] = new HeartsView(equipment, 216, 14);
  elements[nb_elements++] = new RupeesView(equipment, 8, 216);
  elements[nb_elements++] = new MagicBar(equipment, 216, 35);
  elements[nb_elements++] = new ItemIcon(0, savegame, 8, 29);
  elements[nb_elements++] = new ItemIcon(1, savegame, 60, 29);
  elements[nb_elements++] = new SwordIcon(keys_effect, equipment, 10, 29);
  elements[nb_elements++] = new PauseIcon(keys_effect, -3, 7);
  elements[nb_elements++] = new ActionIcon(keys_effect, 23, 51);
  elements[nb_elements++] = new SmallKeysCounter(equipment, 284, 214);
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

  // detect when the game is showing a message
  if (game->is_showing_message() && !showing_message) {
      showing_message = true;

      // a message is shown: hide or move the top-left icons
      elements[3]->set_visible(false); // item 0
      elements[4]->set_visible(false); // item 1
      elements[6]->set_visible(false); // pause icon
      elements[5]->set_position(-18, 29); // sword icon
      elements[7]->set_position(-5, 51); // action icon
  }
  else if (showing_message && !game->is_showing_message()) {
    showing_message = false;

    // a message is shown: hide or move the top-left icons
    elements[3]->set_visible(true); // item 0
    elements[4]->set_visible(true); // item 1
    elements[6]->set_visible(true); // pause icon
    elements[5]->set_position(10, 29); // sword icon
    elements[7]->set_position(23, 51); // action icon
  }

  // update each element
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
