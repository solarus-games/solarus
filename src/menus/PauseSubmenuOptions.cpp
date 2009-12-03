/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
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
#include "Controls.h"
#include "Game.h"
#include "Sprite.h"
#include "ZSDX.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "lowlevel/Sound.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Color.h"

/**
 * Constructor.
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
  video_mode_text->set_font(TextSurface::FONT_STANDARD);

  controls_text = new TextSurface(84, 83, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  controls_text->set_font(TextSurface::FONT_STANDARD);
  controls_text->set_text(StringResource::get_string("options.commands_column"));

  keyboard_text = new TextSurface(153, 83, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  keyboard_text->set_font(TextSurface::FONT_STANDARD);
  keyboard_text->set_text(StringResource::get_string("options.keyboard_column"));

  joypad_text = new TextSurface(229, 83, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_TOP);
  joypad_text->set_font(TextSurface::FONT_STANDARD);
  joypad_text->set_text(StringResource::get_string("options.joypad_column"));

  controls_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 215, 160, 32, 0, 0, 0, 0);
  SDL_SetColorKey(controls_surface, SDL_SRCCOLORKEY, Color::black);
  highest_visible_key = 1;
  controls_visible_y = 0;

  for (int i = 0; i < 9; i++) {

    int y = 2 + 16 * i;

    game_key_texts[i] = new TextSurface(4, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    game_key_texts[i]->set_font(TextSurface::FONT_STANDARD);
    game_key_texts[i]->set_text(controls->get_key_name((Controls::GameKey) (i + 1)));

    keyboard_control_texts[i] = new TextSurface(74, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    keyboard_control_texts[i]->set_font(TextSurface::FONT_STANDARD);

    joypad_control_texts[i] = new TextSurface(143, y, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_TOP);
    joypad_control_texts[i]->set_font(TextSurface::FONT_STANDARD);
  }
  load_control_texts();

  up_arrow_sprite = new Sprite("menus/arrow");
  up_arrow_sprite->set_current_direction(0);

  down_arrow_sprite = new Sprite("menus/arrow");
  down_arrow_sprite->set_current_direction(1);

  // cursor
  cursor_sprite = new Sprite("menus/options_cursor");
  cursor_position = -1;
  set_cursor_position(0);
  customizing = false;

  cursor_sound = ResourceManager::get_sound("cursor");
  ok_sound = ResourceManager::get_sound("danger");

  // action icon
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_CHANGE);
}

/**
 * Destructor.
 */
PauseSubmenuOptions::~PauseSubmenuOptions(void) {

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

  SDL_FreeSurface(controls_surface);

  delete up_arrow_sprite;
  delete down_arrow_sprite;
  delete cursor_sprite;
}

/**
 * Loads the text displayed for each control, for the
 * keyboard and the joypad.
 */
void PauseSubmenuOptions::load_control_texts(void) {

  SDL_FillRect(controls_surface, NULL, Color::black);

  Controls *controls = game->get_controls();
  for (int i = 0; i < 9; i++) {

    Controls::GameKey key = (Controls::GameKey) (i + 1);

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
 * Changes the position of the cursor.
 * @param position new cursor position, from 0 to 9
 */
void PauseSubmenuOptions::set_cursor_position(int position) {

  if (position != this->cursor_position) {
    this->cursor_position = position;

    if (position == 0) { // screen mode
      set_caption_text(caption_strings[0]);
      cursor_sprite_position.x = 104;
      cursor_sprite_position.y = 62;
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

      cursor_sprite_position.x = 55;
      cursor_sprite_position.y = 104 + (position - highest_visible_key) * 16;
      cursor_sprite->set_current_animation("small");
    }
  }
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuOptions::key_pressed(Controls::GameKey key) {
  
  switch (key) {

  case Controls::LEFT:
    pause_menu->show_left_submenu();
    break;

  case Controls::RIGHT:
    pause_menu->show_right_submenu();
    break;

  case Controls::UP:
    cursor_sound->play();
    set_cursor_position((cursor_position + 9) % 10);
    break;

  case Controls::DOWN:
    cursor_sound->play();
    set_cursor_position((cursor_position + 1) % 10);
    break;

  case Controls::ACTION:
    action_key_pressed();
    break;

  default:
    break;
  }
}

/**
 * This function is called when the action key is pressed.
 */
void PauseSubmenuOptions::action_key_pressed(void) {

  ok_sound->play();
  if (cursor_position == 0) {
    VideoManager::get_instance()->switch_video_mode();
  }
  else {
    set_caption_text(caption_strings[2]);
    cursor_sprite->set_current_animation("small_blink");
    Controls::GameKey key_to_customize = (Controls::GameKey) cursor_position;
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
 * Updates this submenu.
 */
void PauseSubmenuOptions::update(void) {

  VideoManager::VideoMode video_mode = VideoManager::get_instance()->get_video_mode();
  video_mode_text->set_text(video_mode_strings[video_mode]);

  cursor_sprite->update();

  if (customizing && controls->is_customization_done()) {
    ok_sound->play();
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
 * Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuOptions::display(SDL_Surface *destination) {
  PauseSubmenu::display(destination);

  // display the cursor
  display_cursor(destination);

  // display the text
  video_mode_text->display(destination);

  controls_text->display(destination);
  keyboard_text->display(destination);
  joypad_text->display(destination);

  SDL_Rect src_position = {0, controls_visible_y, 215, 84};
  static SDL_Rect dst_position = {53, 102};
  SDL_BlitSurface(controls_surface, &src_position, destination, &dst_position);

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
 * Displays the cursor.
 * @param destination the destination surface
 */
void PauseSubmenuOptions::display_cursor(SDL_Surface *destination) {

  cursor_sprite->display(destination, cursor_sprite_position.x, cursor_sprite_position.y);
}

