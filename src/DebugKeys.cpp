/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "MainLoop.h"
#include "Game.h"
#include "Map.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/Movement.h"

/**
 * @brief Constructor.
 * @param main_loop the Solarus main loop object
 */
DebugKeys::DebugKeys(MainLoop& main_loop):
  main_loop(main_loop), game(NULL) {
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
void DebugKeys::set_game(Game* game) {
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

      case InputEvent::KEY_g:
      {
        Hero& hero = game->get_hero();
        Layer layer = hero.get_layer();
        if (layer != LAYER_LOW) {
          layer = Layer(layer - 1);
          game->get_current_map().get_entities().set_entity_layer(hero, layer);
          hero.check_position();
        }
        break;
      }

      case InputEvent::KEY_t:
      {
        Hero& hero = game->get_hero();
        Layer layer = hero.get_layer();
        if (layer != LAYER_HIGH) {
          layer = Layer(layer + 1);
          game->get_current_map().get_entities().set_entity_layer(hero, layer);
          hero.check_position();
        }
        break;
      }

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
  else {
    // no game yet
    if (key == InputEvent::KEY_F1) {
      main_loop.start_game("save1.dat");
    }
    else if (key == InputEvent::KEY_F2) {
      main_loop.start_game("save2.dat");
    }
    else if (key == InputEvent::KEY_F3) {
      main_loop.start_game("save3.dat");
    }
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
    if (game != NULL && game->is_showing_dialog()) {
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

