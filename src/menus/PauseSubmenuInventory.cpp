#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseMenu.h"
#include "Sprite.h"

/**
 * Caption text displayed when an item is selected in the inventory.
 */
static const string item_names[28][6] = {
  // TODO: load this from some external file (for future translation)

  {"Plume"},
  {"Bombes"},
  {"Arc", "Arc & Flèches"},
  {"Boomerang"},
  {"Lanterne"},
  {"Grappin"},
  {"Flacon Magique"},

  {"Bottes de Pégase"},
  {"Miroir Mystique"},
  {"Canne de Somaria"},
  {"Cape Magique"},
  {"Poigne de Fer", "Poigne d'Or"},
  {"Pierres de Feu"},
  {"Flacon Magique"},

  {"Pommes"},
  {"Pains au Chocolat"},
  {"Croissants"},
  {"Tarte aux Pommes", "Lingots d'or", "Edelweiss", "Clé d'Os"},
  {"Palmes"},
  {"Clé Rouge"},
  {"???"},

  {"Clé de Terre"},
  {"Clé de Roc"},
  {"Clé de Fer"},
  {"Clé de Pierre"},
  {"Clé de Bois"},
  {"Clé de Glace"},
  {"???"},
};

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuInventory::PauseSubmenuInventory(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game), cursor_position(0) {

  cursor_sprite = new Sprite("menus/inventory_cursor");
}

/**
 * Destructor.
 */
PauseSubmenuInventory::~PauseSubmenuInventory(void) {
  delete cursor_sprite;
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param keysym the key pressed
 */
void PauseSubmenuInventory::key_pressed(const SDL_keysym &keysym) {

  switch (keysym.sym) {

  case SDLK_LEFT:
    pause_menu->show_left_submenu();
    break;

  case SDLK_RIGHT:
    pause_menu->show_right_submenu();
    break;

  default:
    break;
  }
}

/**
 * Updates this submenu.
 */
void PauseSubmenuInventory::update(void) {
  
}

/**
 * Displays this submenu.
 */
void PauseSubmenuInventory::display(SDL_Surface *destination) {
  PauseSubmenu::display(destination);
}
