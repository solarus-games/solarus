#include "Keyboard.h"
#include "Game.h"
#include "DialogBox.h"
#include "entities/Link.h"
#include "movements/Movement8ByPlayer.h"
#include "menus/PauseMenu.h"

// TODO remove (test code only)
#include "Savegame.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Treasure.h"

/**
 * Constructor.
 * @param game the game
 */
Keyboard::Keyboard(Game *game):
  game(game), link(game->get_link()) {

}

/**
 * Destructor.
 */
Keyboard::~Keyboard(void) {

}

/**
 * This function is called by the game when a key is pressed.
 * Depending on the game state, the appropriate object is notified.
 * @param keysym the key pressed
 */
void Keyboard::key_pressed(const SDL_keysym &keysym) {

  if (!game->is_suspended()) {    

    // if the game is not suspended, most of the keys apply to Link
    if (keysym.sym == SDLK_d) {
      game->set_paused(true);
    }
    else {
      link->key_pressed(keysym);
    }
  }

  // is a message being shown?
  else if (game->is_showing_message()) {
    game->get_dialog_box()->key_pressed(keysym);
  }

  // is the game paused?
  else if (game->is_paused()) {
    game->get_pause_menu()->key_pressed(keysym);
  }

  // TODO temporary (test code)

  Savegame *savegame = game->get_savegame();
  Equipment *equipment = game->get_equipment();
  Treasure *treasure;

  switch (keysym.unicode) {

    // TODO remove
  case SDLK_p:
    game->get_equipment()->add_hearts(2);
    break;
	  
  case SDLK_m:
    game->get_equipment()->remove_hearts(1);
    break;
	  
  case SDLK_o:
    game->get_equipment()->add_rupees(23);
    break;
	  
  case SDLK_l:
    game->get_equipment()->remove_rupees(14);
    break;
	  
  case SDLK_i:
    game->get_equipment()->add_magic(10);
    break;
	  
  case SDLK_k:
    game->get_equipment()->remove_magic(4);
    break;
	  
  case SDLK_j:
    if (!game->get_equipment()->is_magic_decreasing()) {
      game->get_equipment()->start_removing_magic(200);
    }
    else {
      game->get_equipment()->stop_removing_magic();
    }
    break;
	  
  case SDLK_s:
    savegame->save();
    break;

  case SDLK_a:
    // temporary code to test the dialog box
    if (!game->is_showing_message()) {
      game->show_message("msg");
      game->get_dialog_box()->set_variable("test_var", 42);
      game->get_dialog_box()->set_variable("test_name", savegame->get_string(Savegame::PLAYER_NAME));

    }
    break;

  case SDLK_t:
    equipment->give_inventory_item(InventoryItem::BOW);
    equipment->set_max_arrows(30);
    equipment->set_item_assigned(0, InventoryItem::BOW);
    equipment->give_inventory_item(InventoryItem::BOTTLE_2, 6);
    equipment->set_item_assigned(1, InventoryItem::BOTTLE_2);
    equipment->give_inventory_item(InventoryItem::BOMBS);
    equipment->set_max_bombs(30);
    equipment->set_bombs(10);
    equipment->give_inventory_item(InventoryItem::BOOMERANG);
    equipment->give_inventory_item(InventoryItem::LAMP);
    equipment->give_inventory_item(InventoryItem::HOOK_SHOT);
    equipment->give_inventory_item(InventoryItem::PEGASUS_SHOES);
    equipment->give_inventory_item(InventoryItem::BOTTLE_1);
    equipment->give_inventory_item(InventoryItem::GLOVE);
    equipment->give_inventory_item(InventoryItem::PAINS_AU_CHOCOLAT);
    equipment->set_inventory_item_amount(InventoryItem::PAINS_AU_CHOCOLAT, 3);
    equipment->give_inventory_item(InventoryItem::CROISSANTS);
    equipment->give_inventory_item(InventoryItem::FLIPPERS);
    equipment->give_inventory_item(InventoryItem::RED_KEY);
    equipment->give_inventory_item(InventoryItem::CLAY_KEY);
    break;
 
  case SDLK_g:
    equipment->add_arrows(7);
    break;

  case SDLK_b:
    equipment->remove_arrow();
    break;

  case SDLK_r:
    {
      if (game->is_showing_message()) {
	break;
      }
      static int content = Treasure::SWORD_4 -1;
      content++;

      while (content == 1 ||
	     (content >= 8 && content <= 11) ||
	     content == 29 ||
	     (content >= 36 && content <= 49) ||
	     content == 51 ||
	     content == 57 ||
	     content == 60 ||
	     (content >= 75 && content <= 78) ||
	     content == 81) {
	content++;
      }

      treasure = new Treasure((Treasure::Content) content, 13, 0);
      game->give_treasure(treasure);
    }
    break;

  default:
    break;
  }

  switch (keysym.sym) {
  case SDLK_KP7:
    equipment->set_max_magic(0);
    break;
	  
  case SDLK_KP8:
    equipment->set_max_magic(42);
    break;
	  
  case SDLK_KP9:
    equipment->set_max_magic(84);
    break;
	  
  case SDLK_KP1:
    equipment->set_tunic(MAX(equipment->get_tunic() - 1, 0));
    link->rebuild_equipment();
    break;

  case SDLK_KP4:
    equipment->set_tunic(MIN(equipment->get_tunic() + 1, 2));
    link->rebuild_equipment();
    break;
	  
  case SDLK_KP2:
    equipment->set_sword(MAX(equipment->get_sword() - 1, 0));
    link->rebuild_equipment();
    break;
	  
  case SDLK_KP5:
    equipment->set_sword(MIN(equipment->get_sword() + 1, 4));
    link->rebuild_equipment();
    break;
	  
  case SDLK_KP3:
    equipment->set_shield(MAX(equipment->get_shield() - 1, 0));
    link->rebuild_equipment();
    break;
	  
  case SDLK_KP6:
    equipment->set_shield(MIN(equipment->get_shield() + 1, 3));
    link->rebuild_equipment();
    break;
	  
  default:
    break;
  }
}

/**
 * This function is called by the game when a key is released.
 * Depending on the game state, the appropriate object is notified.
 * @param keysym the key pressed
 */
void Keyboard::key_released(const SDL_keysym &keysym) {

  if (!game->is_suspended()) {
    // if the game is not suspended, the keys apply to Link
    link->key_released(keysym);
  }
}
