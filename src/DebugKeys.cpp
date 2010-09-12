/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses	*/
#include "DebugKeys.h"
#include "Solarus.h"
#include "Game.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "entities/Hero.h"
#include "movements/Movement.h"

/**
 * @brief Constructor.
 * @param solarus the application object
 */
DebugKeys::DebugKeys(Solarus *solarus):
  solarus(solarus), game(NULL) {
}

/**
 * @brief Destructor.
 */
DebugKeys::~DebugKeys(void) {

}

/**
 * @brief Sets the current game.
 * @param game the current game
 */
void DebugKeys::set_game(Game *game) {
  this->game = game;
}

/**
 * @brief This function is called when a keyboard key is pressed.
 * @param key the key pressed
 */
void DebugKeys::key_pressed(InputEvent::KeyboardKey key) {

#if SOLARUS_DEBUG_LEVEL >= 2
  // don't consider the debug keys in release mode

  if (game != NULL) {
    Equipment *equipment = game->get_equipment();

    switch (key) {

      case InputEvent::KEY_p:
	equipment->add_life(2);
	break;

      case InputEvent::KEY_m:
	equipment->remove_life(1);
	break;

      case InputEvent::KEY_o:
	equipment->add_money(23);
	break;

      case InputEvent::KEY_l:
	equipment->remove_money(14);
	break;

      case InputEvent::KEY_i:
	equipment->add_magic(10);
	break;

      case InputEvent::KEY_k:
	equipment->remove_magic(4);
	break;

      case InputEvent::KEY_j:
	if (!equipment->is_magic_decreasing()) {
	  equipment->start_removing_magic(200);
	}
	else {
	  equipment->stop_removing_magic();
	}
	break;

      case InputEvent::KEY_t:
	// quest-specific temporary code
	equipment->give_item("bow");
	equipment->set_item_variant("quiver", 1);
	equipment->set_item_variant("bottle_2", 6);
	equipment->give_item("bombs");
	equipment->set_item_variant("bomb_bag", 1);
	equipment->set_item_amount("bombs", 10);
	equipment->give_item("boomerang");
	equipment->give_item("lamp");
	equipment->give_item("hookshot");
	equipment->give_item("speed_shoes");
	equipment->give_item("bottle_1");
	equipment->give_item("glove");
	equipment->give_item("pains_au_chocolat");
	equipment->set_item_amount("pains_au_chocolat", 3);
	equipment->give_item("croissants");
	equipment->give_item("red_key");
	equipment->give_item("clay_key");
	equipment->give_item("world_map");
	equipment->set_item_assigned(0, "boomerang");
	equipment->set_item_assigned(1, "bottle_2");
	break;

      case InputEvent::KEY_g:
	equipment->add_item_amount("bow", 7);
	break;

      case InputEvent::KEY_b:
	equipment->remove_item_amount("bow", 1);
	break;

	/*
	   case InputEvent::KEY_SPACE:
	// almost the feather, actually
	if (game->get_keys_effect()->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE) {
	Hero *hero = game->get_hero();
	hero->start_jumping(hero->get_animation_direction() * 2, 40, true);
	}
	break;
	*/

      case InputEvent::KEY_KP7:
	equipment->set_max_magic(0);
	break;

      case InputEvent::KEY_KP8:
	equipment->set_max_magic(42);
	break;

      case InputEvent::KEY_KP9:
	equipment->set_max_magic(84);
	break;

      case InputEvent::KEY_KP1:
	equipment->set_item_variant("tunic", std::max(equipment->get_item_variant("tunic") - 1, 0));
	game->get_hero()->rebuild_equipment();
	break;

      case InputEvent::KEY_KP4:
	equipment->set_item_variant("tunic", std::min(equipment->get_item_variant("tunic" + 1), 2));
	game->get_hero()->rebuild_equipment();
	break;

      case InputEvent::KEY_KP2:
	equipment->set_item_variant("sword", std::max(equipment->get_item_variant("sword") - 1, 0));
	game->get_hero()->rebuild_equipment();
	break;

      case InputEvent::KEY_KP5:
	equipment->set_item_variant("sword", std::min(equipment->get_item_variant("sword" + 1), 4));
	game->get_hero()->rebuild_equipment();
	break;

      case InputEvent::KEY_KP3:
	equipment->set_item_variant("shield", std::max(equipment->get_item_variant("shield") - 1, 0));
	game->get_hero()->rebuild_equipment();
	break;

      case InputEvent::KEY_KP6:
	equipment->set_item_variant("shield", std::min(equipment->get_item_variant("shield" + 1), 3));
	game->get_hero()->rebuild_equipment();
	break;

      default:
	break;
    }
  }
  else if (key == InputEvent::KEY_TABULATION) {
    // no game yet
    solarus->skip_menus();
  }
#endif
}

/**
 * @brief This function is called when a keyboard key is released.
 * @param key the key released
 */
void DebugKeys::key_released(InputEvent::KeyboardKey key) {
}

/**
 * @brief This function is called repeatedly by the engine.
 */
void DebugKeys::update(void) {

  if (InputEvent::is_shift_down()) {
    if (game != NULL && game->is_showing_message()) {
      game->get_dialog_box()->show_all_now();
    }
  }

#if SOLARUS_DEBUG_LEVEL >= 2
  // traverse walls when control is pressed
  if (game != NULL) {

    Movement *movement = game->get_hero()->get_movement();
    if (movement != NULL) {

      if (InputEvent::is_control_down()) {
	movement->set_ignore_obstacles();
      }
      else {
	movement->restore_ignore_obstacles();
      }
    }
  }
#endif
  
}

