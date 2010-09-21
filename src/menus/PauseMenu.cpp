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
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "menus/PauseMenu.h"
#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseSubmenuMap.h"
#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseSubmenuOptions.h"
#include "Game.h"
#include "Savegame.h"
#include "Sprite.h"
#include "StringResource.h"
#include "lowlevel/Color.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Surface.h"

/**
 * @brief Opens a pause menu.
 * @param game the game
 */
PauseMenu::PauseMenu(Game *game):
  game(game), savegame(game->get_savegame()), keys_effect(game->get_keys_effect()) {

  this->current_submenu = NULL;
  this->backgrounds_surface = new Surface("pause_submenus.png", Surface::DIR_LANGUAGE);
  backgrounds_surface->set_opacity(216);

  this->save_dialog_sprite = new Sprite("menus/pause_save_dialog");
  this->save_dialog_state = 0;

  game->play_sound("pause_open");
  keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_RETURN);
  keys_effect->save_action_key_effect();
  keys_effect->save_sword_key_effect();

  question_text[0] = new TextSurface(160, 112, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  question_text[0]->set_text_color(8, 8, 8);
  question_text[1] = new TextSurface(160, 128, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  question_text[1]->set_text_color(8, 8, 8);

  answer_text[0] = new TextSurface(100, 148, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  answer_text[0]->set_text_color(8, 8, 8);
  answer_text[0]->set_text(StringResource::get_string("save_dialog.yes"));
  answer_text[1] = new TextSurface(219, 148, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  answer_text[1]->set_text_color(8, 8, 8);
  answer_text[1]->set_text(StringResource::get_string("save_dialog.no"));

  set_current_submenu(savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU));
}

/**
 * @brief Destructor.
 */
PauseMenu::~PauseMenu() {
  delete current_submenu;
  delete backgrounds_surface;
  delete save_dialog_sprite;

  delete question_text[0];
  delete question_text[1];
  delete answer_text[0];
  delete answer_text[1];
}

/**
 * @brief Exits the pause menu.
 */
void PauseMenu::quit() {

  game->play_sound("pause_closed");
  keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_PAUSE);
  keys_effect->restore_action_key_effect();
  keys_effect->restore_sword_key_effect();
}

/**
 * @brief This function is called when a key is pressed.
 * @param key the key pressed
 */
void PauseMenu::key_pressed(GameControls::GameKey key) {
 
  if (key == GameControls::PAUSE) {
    quit();
    game->set_paused(false);
  }

  // the user is in one of the submenus
  else if (save_dialog_state == 0) {

    if (key == GameControls::SWORD) {

      game->play_sound("message_end");

      save_dialog_state = 1;
      save_dialog_choice = 0;
      save_dialog_sprite->set_current_animation("left");

      question_text[0]->set_text(StringResource::get_string("save_dialog.save_question_0"));
      question_text[1]->set_text(StringResource::get_string("save_dialog.save_question_1"));

      action_key_effect_saved = keys_effect->get_action_key_effect();
      sword_key_effect_saved = keys_effect->get_sword_key_effect();
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_VALIDATE);
      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_VALIDATE);
    }
    else {
      current_submenu->key_pressed(key);
    }
  }

  // the dialog box to save is visible
  else if (key == GameControls::LEFT || key == GameControls::RIGHT) {
    // move the cursor

    game->play_sound("cursor");
    save_dialog_choice = 1 - save_dialog_choice;
    save_dialog_sprite->set_current_animation(save_dialog_choice == 0 ? "left" : "right");
  }
  else if (key == GameControls::ACTION || key == GameControls::SWORD) {
    // validate a choice

    if (save_dialog_state == 1) {
      save_dialog_state = 2;
 
      if (save_dialog_choice == 0) {
	savegame->save();
	game->play_sound("piece_of_heart");
      }
      else {
	game->play_sound("danger");
      }

      question_text[0]->set_text(StringResource::get_string("save_dialog.continue_question_0"));
      question_text[1]->set_text(StringResource::get_string("save_dialog.continue_question_1"));

      save_dialog_choice = 0;
      save_dialog_sprite->set_current_animation("left");
    }
    else {
      game->play_sound("danger");

      save_dialog_state = 0;
      keys_effect->set_action_key_effect(action_key_effect_saved);
      keys_effect->set_sword_key_effect(sword_key_effect_saved);

      if (save_dialog_choice == 1) {
	game->reset();
	quit();
	game->set_paused(false);
      }
    }
  }
}

/**
 * @brief Updates the pause menu.
 */
void PauseMenu::update() {
  current_submenu->update();
}

/**
 * @brief Displays the pause menu.
 * @param destination the surface to draw
 */
void PauseMenu::display(Surface *destination) {

  // display the background for the current submenu
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  Rectangle src_position(320 * submenu_index, 0, 320, 240);
  backgrounds_surface->blit(src_position, destination);

  // display the current submenu content
  current_submenu->display(destination);

  // display the save dialog
  if (save_dialog_state > 0) {
    save_dialog_sprite->display(destination, 50, 87);
    question_text[0]->display(destination);
    question_text[1]->display(destination);
    answer_text[0]->display(destination);
    answer_text[1]->display(destination);
  }
}

/**
 * @brief Moves to the specified submenu.
 * @param submenu_index the submenu to show
 */
void PauseMenu::set_current_submenu(int submenu_index) {

  // show the default action and sword keys
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SAVE);

  // go to the specified submenu
  savegame->set_integer(Savegame::PAUSE_LAST_SUBMENU, submenu_index);

  if (current_submenu != NULL) {
    delete current_submenu;
  }

  switch (submenu_index) {

  case 0:
    current_submenu = new PauseSubmenuInventory(this, game);
    break;

  case 1:
    current_submenu = new PauseSubmenuMap(this, game);
    break;

  case 2:
    current_submenu = new PauseSubmenuQuestStatus(this, game);
    break;

  case 3:
    current_submenu = new PauseSubmenuOptions(this, game);
    break;
  }
}

/**
 * @brief Shows the submenu located at the left side from the current one.
 */
void PauseMenu::show_left_submenu() {

  game->play_sound("pause_closed");
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  set_current_submenu((submenu_index + 3) % 4);
}

/**
 * @brief Shows the submenu located at the right side from the current one.
 */
void PauseMenu::show_right_submenu() {

  game->play_sound("pause_closed");
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  set_current_submenu((submenu_index + 1) % 4);
}

