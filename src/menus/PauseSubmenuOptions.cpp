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
#include "menus/PauseSubmenuOptions.h"
#include "menus/PauseMenu.h"
#include "StringResource.h"
#include "Game.h"
#include "Sprite.h"
#include "KeysEffect.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Sound.h"
#include <sstream>

/**
 * @brief Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuOptions::PauseSubmenuOptions(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game), controls(game->get_controls()) {

  // get the strings
  video_mode_strings = new std::string[VideoManager::NB_MODES];
  for (int i = 0; i < VideoManager::NB_MODES; i++) {
    std::ostringstream oss;
    oss << "options.video_mode_" << i;
    video_mode_strings[i] = StringResource::get_string(oss.str());
  }
  caption_strings[0] = StringResource::get_string("options.caption.press_action_change_mode");
  caption_strings[1] = StringResource::get_string("options.caption.press_action_customize_key");
  caption_strings[2] = StringResource::get_string("options.caption.press_key");

  // create the text surfaces
  video_mode_text = new TextSurface(264, 62, TextSurface::ALIGN_RIGHT, TextSurface::ALIGN_TOP);
  video_mode_text->set_font("fixed");

  controls_text = new TextSurface(84, 83, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  controls_text->set_font("fixed");
  controls_text->set_text(StringResource::get_string("options.commands_column"));

  keyboard_text = new TextSurface(153, 83, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  keyboard_text->set_font("fixed");
  keyboard_text->set_text(StringResource::get_string("options.keyboard_column"));

  joypad_text = new TextSurface(229, 83, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  joypad_text->set_font("fixed");
  joypad_text->set_text(StringResource::get_string("options.joypad_column"));

  controls_surface = new Surface(215, 160);
  controls_surface->set_transparency_color(Color::get_black());
  highest_visible_key = 1;
  controls_visible_y = 0;

  for (int i = 0; i < 9; i++) {

    int y = 2 + 16 * i;

    game_key_texts[i] = new TextSurface(4, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    game_key_texts[i]->set_font("fixed");
    game_key_texts[i]->set_text(controls->get_key_name((GameControls::GameKey) (i + 1)));

    keyboard_control_texts[i] = new TextSurface(74, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    keyboard_control_texts[i]->set_font("fixed");

    joypad_control_texts[i] = new TextSurface(143, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    joypad_control_texts[i]->set_font("fixed");
  }
  load_control_texts();

  up_arrow_sprite = new Sprite("menus/arrow");
  up_arrow_sprite->set_current_direction(1);

  down_arrow_sprite = new Sprite("menus/arrow");
  down_arrow_sprite->set_current_direction(3);

  // cursor
  cursor_sprite = new Sprite("menus/options_cursor");
  cursor_position = -1;
  set_cursor_position(0);
  customizing = false;

  // action icon
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_CHANGE);
}

/**
 * @brief Destructor.
 */
PauseSubmenuOptions::~PauseSubmenuOptions() {

  delete[] video_mode_strings;
  delete video_mode_text;

  delete controls_text;
  delete keyboard_text;
  delete joypad_text;

  for (int i = 0; i < 9; i++) {
    delete game_key_texts[i];
    delete keyboard_control_texts[i];
    delete joypad_control_texts[i];
  }

  delete controls_surface;
  delete up_arrow_sprite;
  delete down_arrow_sprite;
  delete cursor_sprite;
}

/**
 * @brief Loads the text displayed for each control, for the
 * keyboard and the joypad.
 */
void PauseSubmenuOptions::load_control_texts() {
  
  GameControls *controls = game->get_controls();

  controls_surface->fill_with_color(Color::get_black());
  for (int i = 0; i < 9; i++) {

    GameControls::GameKey key = (GameControls::GameKey) (i + 1);

    const std::string &keyboard_text = controls->get_keyboard_string(key);
    keyboard_control_texts[i]->set_text(keyboard_text.substr(0, 9));

    const std::string &joypad_text = controls->get_joypad_string(key);
    joypad_control_texts[i]->set_text(joypad_text.substr(0, 9));

    game_key_texts[i]->display(controls_surface);
    keyboard_control_texts[i]->display(controls_surface);
    joypad_control_texts[i]->display(controls_surface);
  }
}

/**
 * @brief Changes the position of the cursor.
 * @param position new cursor position, from 0 to 9
 */
void PauseSubmenuOptions::set_cursor_position(int position) {

  if (position != this->cursor_position) {
    this->cursor_position = position;

    if (position == 0) { // screen mode
      set_caption_text(caption_strings[0]);
      cursor_sprite_position.set_xy(104, 62);
      cursor_sprite->set_current_animation("big");
    }
    else { // key customization
      set_caption_text(caption_strings[1]);

      // make sure the selected key is visible
      while (position < highest_visible_key) {
	highest_visible_key--;
	controls_visible_y -= 16;
      }

      while (position > highest_visible_key + 4) {
	highest_visible_key++;
	controls_visible_y += 16;
      }

      cursor_sprite_position.set_x(55);
      cursor_sprite_position.set_y(104 + (position - highest_visible_key) * 16);
      cursor_sprite->set_current_animation("small");
    }
  }
}

/**
 * @brief This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuOptions::key_pressed(GameControls::GameKey key) {
  
  switch (key) {

  case GameControls::LEFT:
    pause_menu->show_left_submenu();
    break;

  case GameControls::RIGHT:
    pause_menu->show_right_submenu();
    break;

  case GameControls::UP:
    Sound::play("cursor");
    set_cursor_position((cursor_position + 9) % 10);
    break;

  case GameControls::DOWN:
    Sound::play("cursor");
    set_cursor_position((cursor_position + 1) % 10);
    break;

  case GameControls::ACTION:
    action_key_pressed();
    break;

  default:
    break;
  }
}

/**
 * @brief This function is called when the action key is pressed.
 */
void PauseSubmenuOptions::action_key_pressed() {

  Sound::play("danger");
  if (cursor_position == 0) {
    VideoManager::get_instance()->switch_video_mode();
  }
  else {
    set_caption_text(caption_strings[2]);
    cursor_sprite->set_current_animation("small_blink");
    GameControls::GameKey key_to_customize = (GameControls::GameKey) cursor_position;
    controls->customize(key_to_customize);
    customizing = true;

    KeysEffect *keys_effect = game->get_keys_effect();
    keys_effect->set_item_keys_enabled(false);
    keys_effect->set_action_key_enabled(false);
    keys_effect->set_sword_key_enabled(false);
    keys_effect->set_pause_key_enabled(false);
  }
}

/**
 * @brief Updates this submenu.
 */
void PauseSubmenuOptions::update() {

  VideoManager::VideoMode video_mode = VideoManager::get_instance()->get_video_mode();
  video_mode_text->set_text(video_mode_strings[video_mode]);

  cursor_sprite->update();

  if (customizing && controls->is_customization_done()) {
    Sound::play("danger");
    customizing = false;
    set_caption_text(caption_strings[1]);
    cursor_sprite->set_current_animation("small");
    load_control_texts();

    KeysEffect *keys_effect = game->get_keys_effect();
    keys_effect->set_item_keys_enabled(true);
    keys_effect->set_action_key_enabled(true);
    keys_effect->set_sword_key_enabled(true);
    keys_effect->set_pause_key_enabled(true);
  }
}

/**
 * @brief Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuOptions::display(Surface *destination) {
  PauseSubmenu::display(destination);

  // display the cursor
  display_cursor(destination);

  // display the text
  video_mode_text->display(destination);

  controls_text->display(destination);
  keyboard_text->display(destination);
  joypad_text->display(destination);

  Rectangle src_position(0, controls_visible_y, 215, 84);
  static Rectangle dst_position(53, 102);
  controls_surface->blit(src_position, destination, dst_position);

  // display the arrows
  if (controls_visible_y > 0) {
    up_arrow_sprite->display(destination, 96, 96);
    up_arrow_sprite->display(destination, 211, 96);
  }

  if (controls_visible_y < 60) {
    down_arrow_sprite->display(destination, 96, 182);
    down_arrow_sprite->display(destination, 211, 182);
  }
}

/**
 * @brief Displays the cursor.
 * @param destination the destination surface
 */
void PauseSubmenuOptions::display_cursor(Surface *destination) {
  cursor_sprite->display(destination, cursor_sprite_position.get_x(), cursor_sprite_position.get_y());
}

