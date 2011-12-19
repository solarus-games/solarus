/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
DebugKeys::DebugKeys(Solarus &solarus):
  solarus(solarus), game(NULL) {
}

/**
 * @brief Destructor.
 */
DebugKeys::~DebugKeys() {

}

/**
 * @brief Sets the current game.
 * @param game the current game, or NULL if there is no game
 */
void DebugKeys::set_game(Game *game) {
  this->game = game;
}

/**
 * @brief This function is called when a keyboard key is pressed.
 * @param key the key pressed
 */
void DebugKeys::key_pressed(InputEvent::KeyboardKey key) {

#ifdef SOLARUS_DEBUG_KEYS
  // don't consider the debug keys in release mode

  if (game != NULL) {
    Equipment &equipment = game->get_equipment();

    switch (key) {

      case InputEvent::KEY_p:
        equipment.add_life(12);
        break;

      case InputEvent::KEY_m:
        equipment.remove_life(1);
        break;

      case InputEvent::KEY_o:
        equipment.add_money(23);
        break;

      case InputEvent::KEY_l:
        equipment.remove_money(14);
        break;

      case InputEvent::KEY_i:
        equipment.add_magic(10);
        break;

      case InputEvent::KEY_k:
        equipment.remove_magic(4);
        break;

      case InputEvent::KEY_j:
        if (!equipment.is_magic_decreasing()) {
          equipment.start_removing_magic(200);
        }
        else {
          equipment.stop_removing_magic();
        }
        break;

      case InputEvent::KEY_t:
        // quest-specific temporary code
        /*
        if (!game->is_paused()) {
          equipment.add_item("bow", 1);
          equipment.add_item("bombs_counter", 1);
          equipment.add_item("boomerang", 1);
          equipment.add_item("lamp", 1);
          equipment.add_item("hookshot", 1);
          equipment.add_item("pegasus_shoes", 1);
          equipment.add_item("bottle_1", 1);
          equipment.add_item("glove", 2);
          equipment.add_item("pain_au_chocolat", 1);
          equipment.add_item("croissant", 1);
          equipment.add_item("clay_key", 1);
          equipment.add_item("world_map", 1);
          equipment.add_item("feather", 1);
          equipment.add_item("flippers", 1);
          equipment.add_item("magic_cape", 1);
          equipment.set_item_assigned(0, "bow");
          equipment.set_item_assigned(1, "feather");
        }
        */
        break;

      case InputEvent::KEY_KP7:
        equipment.set_max_magic(0);
        break;

      case InputEvent::KEY_KP8:
        equipment.set_max_magic(42);
        break;

      case InputEvent::KEY_KP9:
        equipment.set_max_magic(84);
        break;

      case InputEvent::KEY_KP1:
        equipment.set_ability("tunic", std::max(equipment.get_ability("tunic") - 1, 1));
        game->get_hero().rebuild_equipment();
        break;

      case InputEvent::KEY_KP4:
        equipment.set_ability("tunic", std::min(equipment.get_ability("tunic") + 1, 3));
        game->get_hero().rebuild_equipment();
        break;

      case InputEvent::KEY_KP2:
        equipment.set_ability("sword", std::max(equipment.get_ability("sword") - 1, 0));
        game->get_hero().rebuild_equipment();
        break;

      case InputEvent::KEY_KP5:
        equipment.set_ability("sword", std::min(equipment.get_ability("sword") + 1, 4));
        game->get_hero().rebuild_equipment();
        break;

      case InputEvent::KEY_KP3:
        equipment.set_ability("shield", std::max(equipment.get_ability("shield") - 1, 0));
        game->get_hero().rebuild_equipment();
        break;

      case InputEvent::KEY_KP6:
        equipment.set_ability("shield", std::min(equipment.get_ability("shield") + 1, 3));
        game->get_hero().rebuild_equipment();
        break;

      default:
        break;
    }
  }
  else if (key == InputEvent::KEY_TABULATION) {
    // no game yet
    solarus.skip_menus();
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
void DebugKeys::update() {

#ifdef SOLARUS_DEBUG_KEYS
  if (InputEvent::is_shift_down()) {
    if (game != NULL && game->is_showing_message()) {
      game->get_dialog_box().show_all_now();
    }
  }

  // traverse walls when control is pressed
  if (game != NULL) {

    Movement *movement = game->get_hero().get_movement();
    if (movement != NULL) {

      if (InputEvent::is_control_down()) {
        movement->set_ignore_obstacles(true);
      }
      else {
        movement->restore_default_ignore_obstacles();
      }
    }
  }
#endif

}

