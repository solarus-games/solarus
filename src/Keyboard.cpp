#include "Keyboard.h"
#include "Game.h"
#include "DialogBox.h"
#include "entities/Link.h"
#include "movements/Movement8ByPlayer.h"

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
    // if the game is not suspended, the keys apply to Link

    switch (keysym.sym) {

      // action key
    case SDLK_SPACE:
      link->action_key_pressed();
      break;

      // sword key
    case SDLK_c:
      link->sword_key_pressed();
      break;

      // move Link
    case SDLK_RIGHT:
      link->get_movement()->start_right();
      break;

    case SDLK_UP:
      link->get_movement()->start_up();
      break;

    case SDLK_LEFT:
      link->get_movement()->start_left();
      break;

    case SDLK_DOWN:
      link->get_movement()->start_down();
      break;

    default:
      break;
    }
  }
  else if (game->is_showing_message()) { // is a message being shown?

    switch (keysym.sym) {

      // action key
    case SDLK_SPACE:
      game->get_dialog_box()->action_key_pressed();
      break;

      // sword key
    case SDLK_c:
      game->get_dialog_box()->sword_key_pressed();
      break;

    case SDLK_UP:
    case SDLK_DOWN:
      game->get_dialog_box()->up_or_down_key_pressed();
      break;

    default:
      break;
    }
  }

  // TODO temporary (test code)

  Savegame *savegame = game->get_savegame();
  KeysEffect *keys_effect = game->get_keys_effect();
  Equipment *equipment = savegame->get_equipment();
  Treasure *treasure;

  switch (keysym.unicode) {

    // TODO remove
  case SDLK_p:
    savegame->get_equipment()->add_hearts(2);
    break;
	  
  case SDLK_m:
    savegame->get_equipment()->remove_hearts(1);
    break;
	  
  case SDLK_o:
    savegame->get_equipment()->add_rupees(23);
    break;
	  
  case SDLK_l:
    savegame->get_equipment()->remove_rupees(14);
    break;
	  
  case SDLK_i:
    savegame->get_equipment()->add_magic(10);
    break;
	  
  case SDLK_k:
    savegame->get_equipment()->remove_magic(4);
    break;
	  
  case SDLK_j:
    if (!savegame->get_equipment()->is_magic_decreasing()) {
      savegame->get_equipment()->start_removing_magic(200);
    }
    else {
      savegame->get_equipment()->stop_removing_magic();
    }
    break;
	  
  case SDLK_s:
    savegame->save();
    break;
	  
  case SDLK_d:
	  
    // temoporary code to make like the game is paused
    if (!game->is_paused()) {
	    
      ResourceManager::get_sound("pause_open")->play();
      keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_RETURN);
      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SAVE);
      game->set_paused(true);
    }
    else {
      ResourceManager::get_sound("pause_closed")->play();
      keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_PAUSE);
      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SWORD);
      game->set_paused(false);
    }
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

      link->give_treasure(treasure);
    }
    break;

  case SDLK_f:
    treasure = new Treasure(Treasure::ARROWS, 1, 43);
    link->give_treasure(treasure);
    break;

  case SDLK_v:
    treasure = new Treasure(Treasure::ARROWS, 10, 43);
    link->give_treasure(treasure);
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

    switch (keysym.sym) {
	
      // move Link
    case SDLK_RIGHT:
      link->get_movement()->stop_right();
      break;

    case SDLK_UP:
      link->get_movement()->stop_up();
      break;

    case SDLK_LEFT:
      link->get_movement()->stop_left();
      break;

    case SDLK_DOWN:
      link->get_movement()->stop_down();
      break;

    default:
      break;
    }
  }
}
