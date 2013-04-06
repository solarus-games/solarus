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
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "DebugKeys.h"
#include "MainLoop.h"
#include "Game.h"
#include "DialogBox.h"
#include "entities/Hero.h"
#include "movements/Movement.h"

/**
 * @brief Constructor.
 * @param main_loop The Solarus main loop object.
 */
DebugKeys::DebugKeys(MainLoop& main_loop):
  main_loop(main_loop) {
}

/**
 * @brief Destructor.
 */
DebugKeys::~DebugKeys() {

}

/**
 * @brief Returns the Solarus main loop object.
 * @return The Solarus main loop object.
 */
MainLoop& DebugKeys::get_main_loop() {
  return main_loop;
}

/**
 * @brief This function is called repeatedly by the engine.
 */
void DebugKeys::update() {

#ifdef SOLARUS_DEBUG_KEYS
  Game* game = main_loop.get_game();
  if (InputEvent::is_shift_down()) {
    if (game != NULL && game->is_dialog_enabled()) {
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

