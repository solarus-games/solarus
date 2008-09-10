#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseMenu.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Game.h"
#include "Equipment.h"
#include "Savegame.h"
#include "Counter.h"

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
  PauseSubmenu(pause_menu, game), cursor_row(0), cursor_column(0) {

  Savegame *savegame = game->get_savegame();
  equipment = game->get_equipment();

  cursor_sprite = new Sprite("menus/pause_cursor");
  cursor_sound = ResourceManager::get_sound("cursor");

  items_img = ResourceManager::load_image("hud/inventory_items.png");

  // set the counters
  for (int k = 0; k < 28; k++) {

    // get the item, its counter property and the possession state    
    InventoryItem::ItemId item_id = (InventoryItem::ItemId) k;
    InventoryItem *item = InventoryItem::get_item(item_id);
    int variant = equipment->has_inventory_item(item_id);

    if (variant != 0 && item->has_counter()) {

      // if the player has the item and this item has a counter, we show a counter

      int amount = savegame->get_integer(item->get_counter_index());
      int x = 60 + (k % 7) * 32;
      int y = 81 + (k / 7) * 32;

      counters[k] = new Counter(2, false, x, y);
      counters[k]->set_value(amount);
    }
    else {
      counters[k] = NULL;
    }
  }
}

/**
 * Destructor.
 */
PauseSubmenuInventory::~PauseSubmenuInventory(void) {
  delete cursor_sprite;
  SDL_FreeSurface(items_img);

  for (int k = 0; k < 28; k++) {
    if (counters[k] != NULL) {
      delete counters[k];
    }
  }
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param keysym the key pressed
 */
void PauseSubmenuInventory::key_pressed(const SDL_keysym &keysym) {

  switch (keysym.sym) {

  case SDLK_LEFT:
    if (cursor_column == 0) {
      pause_menu->show_left_submenu();
    }
    else {
      cursor_sound->play();
      cursor_column--;
      // TODO set_caption_name
    }
    break;

  case SDLK_RIGHT:
    if (cursor_column == 6) {
      pause_menu->show_right_submenu();
    }
    else {
      cursor_sound->play();
      cursor_column++;
    }
    break;

  case SDLK_UP:
    cursor_sound->play();
    cursor_row = (cursor_row + 3) % 4;
    break;

  case SDLK_DOWN:
    cursor_sound->play();
    cursor_row = (cursor_row + 1) % 4;
    break;

  default:
    break;
  }
}

/**
 * Updates this submenu.
 * The items displayed and their counters are not updated since the
 * equipment does not change while the game is paused.
 */
void PauseSubmenuInventory::update(void) {
  cursor_sprite->update_current_frame();
}

/**
 * Displays this submenu.
 */
void PauseSubmenuInventory::display(SDL_Surface *destination) {

  // display the common part
  PauseSubmenu::display(destination);

  // display each inventory item
  SDL_Rect src_position = {0, 0, 16, 16};
  SDL_Rect dst_position = {0, 69, 0, 0};

  int k = 0;
  for (int i = 0; i < 4; i++) {

    dst_position.x = 56;

    for (int j = 0; j < 7; j++, k++) {

      // get the possession state of this item
      InventoryItem::ItemId item_id = (InventoryItem::ItemId) k;
      int variant = equipment->has_inventory_item(item_id);

      if (variant > 0) {

	// the player has this item, display the variant he has
	src_position.x = 16 * item_id;
	src_position.y = 16 * (variant - 1);
	SDL_BlitSurface(items_img, &src_position, destination, &dst_position);

	// display the counter (if any)
	if (counters[k] != NULL) {
	  counters[k]->display(destination);
	}
      }

      dst_position.x += 32;
    }
    dst_position.y += 32;
  }

  // display the cursor
  int x = 64 + 32 * cursor_column;
  int y = 77 + 32 * cursor_row;

  cursor_sprite->display(destination, x, y);
}
