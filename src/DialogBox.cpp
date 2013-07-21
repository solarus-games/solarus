/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "DialogBox.h"
#include "DialogResource.h"
#include "Game.h"
#include "lowlevel/TextSurface.h"
#include "lua/LuaContext.h"

/**
 * \brief Creates a new dialog box.
 * \param game The game this dialog box belongs to.
 */
DialogBox::DialogBox(Game& game):
  game(game),
  callback_ref(LUA_REFNIL) {

  for (int i = 0; i < nb_visible_lines; i++) {
    line_surfaces[i] = new TextSurface(0, 0, TextSurface::ALIGN_LEFT,
        TextSurface::ALIGN_TOP);
  }
}

/**
 * \brief Destructor.
 */
DialogBox::~DialogBox() {

  for (int i = 0; i < nb_visible_lines; i++) {
    delete line_surfaces[i];
  }
  game.get_lua_context().cancel_callback(callback_ref);
}

/**
 * \brief Returns the game where this dialog box is displayed.
 * \return the current game
 */
Game& DialogBox::get_game() {
  return game;
}

/**
 * \brief Returns whether the dialog box is currently active.
 * \return true if the dialog box is enabled
 */
bool DialogBox::is_enabled() {

  return dialog_id.size() > 0;
}

/**
 * \brief Returns the id of the current dialog.
 * \return the id of the dialog currently shown
 */
const std::string& DialogBox::get_dialog_id() {
  return dialog_id;
}

/**
 * \brief Opens the dialog box to show a dialog.
 *
 * No other dialog should be already running.
 *
 * \param dialog_id Id of the dialog to show.
 * \param info_ref Lua ref to an optional info parameter to pass to the
 * dialog box, or LUA_REFNIL.
 * \param callback_ref Lua ref to a function to call when the dialog finishes,
 * or LUA_REFNIL.
 */
void DialogBox::open(const std::string& dialog_id,
    int info_ref, int callback_ref) {

  Debug::check_assertion(!is_enabled(), "A dialog is already active");

  this->dialog_id = dialog_id;
  this->dialog = DialogResource::get_dialog(dialog_id);
  this->callback_ref = callback_ref;

  // A dialog was just started: notify Lua.
  bool handled = game.get_lua_context().notify_dialog_started(
      game, dialog_id, info_ref);

  if (!handled) {
    // TODO show a built-in default dialog box.
  }
}

/**
 * \brief Closes the dialog box.
 * \param status_ref Lua ref to a status value to return to the start_dialog
 * callback, or LUA_REFNIL. "skipped" means that the dialog was canceled by
 * the user.
 */
void DialogBox::close(int status_ref) {

  Debug::check_assertion(is_enabled(), "No dialog is active");

  int callback_ref = this->callback_ref;
  const std::string& dialog_id = this->dialog_id;
  this->callback_ref = LUA_REFNIL;
  this->dialog_id = "";

  // A dialog was just finished: notify Lua.
  game.get_lua_context().notify_dialog_finished(
      game, dialog_id, callback_ref, status_ref);
}

