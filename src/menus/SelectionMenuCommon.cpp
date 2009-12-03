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
#include "menus/SelectionMenuCommon.h"
#include "Sprite.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "hud/ActionIcon.h"
#include "hud/SwordIcon.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "hud/HeartsView.h"
#include "Savegame.h"
#include "lowlevel/Color.h"

/**
 * Creates the common part of all selection menu phases.
 */
SelectionMenuCommon::SelectionMenuCommon(void) {

  // fill the arrays
  for (int i = 0; i < 3; i++) {
    savegames[i] = NULL;
    text_player_names[i] = NULL;
    hearts_views[i] = NULL;
  }

  // load the savegames
  read_savegames();

  // load the images
  img_cloud = ResourceManager::load_image("menus/selection_menu_cloud.png");
  img_background = ResourceManager::load_image("menus/selection_menu_background.png");
  img_save_container = ResourceManager::load_image("menus/selection_menu_save_container.png");
  img_option_container = ResourceManager::load_image("menus/selection_menu_option_container.png");

  for (int i = 0; i < 3; i++) {
    std::ostringstream oss;
    oss << "menus/selection_menu_save" << (i + 1) << ".png";
    img_numbers[i] = ResourceManager::load_image(oss.str());
  }

  cursor = new Sprite("menus/selection_menu_cursor");
  background_color = Color::create(104, 144, 240);

  // texts
  text_option1 = new TextSurface(90, 172, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  text_option2 = new TextSurface(198, 172, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  text_title = new TextSurface(160, 54, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  text_title->set_font(TextSurface::FONT_STANDARD);

  // icons
  keys_effect = new KeysEffect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_VALIDATE);
  keys_effect->set_sword_key_enabled(false);
  action_icon = new ActionIcon(keys_effect, 13, 31);
  sword_icon = new SwordIcon(keys_effect, NULL, 0, 9);

  // sounds
  cursor_sound = ResourceManager::get_sound("cursor");
  ok_sound = ResourceManager::get_sound("ok");
  error_sound = ResourceManager::get_sound("wrong");

  // music
  music = ResourceManager::get_music("game_over.spc");
  music->play();

  // initialize the clouds
  initialize_clouds();
}

/**
 * Destructor.
 */
SelectionMenuCommon::~SelectionMenuCommon(void) {
  SDL_FreeSurface(img_cloud);
  SDL_FreeSurface(img_background);
  SDL_FreeSurface(img_save_container);
  SDL_FreeSurface(img_option_container);

  delete text_option1;
  delete text_option2;
  delete text_title;

  for (int i = 0; i < 3; i++) {
    SDL_FreeSurface(img_numbers[i]);
    delete text_player_names[i];
    delete hearts_views[i];
    delete savegames[i];
  }

  delete action_icon;
  delete sword_icon;
  delete keys_effect;

  delete cursor;

  music->stop();
}

/**
 * Initializes the clouds position.
 */
void SelectionMenuCommon::initialize_clouds(void) {

  next_cloud_move = SDL_GetTicks();

  int i;

  for (i = 0; i < 16; i++) {
    cloud_positions[i].w = 80;
    cloud_positions[i].h = 44;
  }

  i = 0;

  cloud_positions[i].x = 20;
  cloud_positions[i].y = 40;
  i++;

  cloud_positions[i].x = 50;
  cloud_positions[i].y = 160;
  i++;

  cloud_positions[i].x = 160;
  cloud_positions[i].y = 30;
  i++;

  cloud_positions[i].x = 270;
  cloud_positions[i].y = 200;
  i++;

  cloud_positions[i].x = 200;
  cloud_positions[i].y = 120;
  i++;

  cloud_positions[i].x = 90;
  cloud_positions[i].y = 120;
  i++;

  cloud_positions[i].x = 300;
  cloud_positions[i].y = 100;
  i++;

  cloud_positions[i].x = 240;
  cloud_positions[i].y = 10;
  i++;

  cloud_positions[i].x = 60;
  cloud_positions[i].y = 190;
  i++;

  cloud_positions[i].x = 150;
  cloud_positions[i].y = 210;
  i++;

  cloud_positions[i].x = 310;
  cloud_positions[i].y = 220;
  i++;

  cloud_positions[i].x = 70;
  cloud_positions[i].y = 20;
  i++;

  cloud_positions[i].x = 130;
  cloud_positions[i].y = 180;
  i++;

  cloud_positions[i].x = 200;
  cloud_positions[i].y = 70;
  i++;

  cloud_positions[i].x = 20;
  cloud_positions[i].y = 120;
  i++;

  cloud_positions[i].x = 170;
  cloud_positions[i].y = 220;
  i++;
}

/**
 * Loads (or reloads) the data of the 3 savegames and creates the surfaces to display.
 */
void SelectionMenuCommon::read_savegames(void) {

  // load the 3 saves
  std::string file_name;
  for (int i = 0; i < 3; i++) {

    std::ostringstream oss;
    oss << "save" << (i + 1) << ".zsd";
    file_name = oss.str();

    if (savegames[i] != NULL) {
      delete savegames[i];
    }
    savegames[i] = new Savegame(file_name);

    // get the data

    // player name
    std::string player_name;
    if (!savegames[i]->is_empty()) {
      player_name = savegames[i]->get_string(Savegame::PLAYER_NAME);
    }
    else {
      player_name = "- Vide -";
    }

    if (text_player_names[i] != NULL) {
      delete text_player_names[i];
    }

    text_player_names[i] = new TextSurface(87, 88 + i * 27,
					   TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
    text_player_names[i]->set_text(player_name);

    // hearts
    if (hearts_views[i] != NULL) {
      delete hearts_views[i];
    }

    hearts_views[i] = new HeartsView(savegames[i]->get_equipment(), 168, 78 + i * 27);
  }
}

/**
 * Updates the common part of the selection menu.
 * This function is called by the update() function of
 * the current selection menu phase.
 */
void SelectionMenuCommon::update(void) {

  // move the clouds
  uint32_t now = SDL_GetTicks();
  while (now >= next_cloud_move) {
    
    for (int i = 0; i < 16; i++) {
      cloud_positions[i].x += 1;
      cloud_positions[i].y -= 1;

      if (cloud_positions[i].x >= 320) {
	cloud_positions[i].x = 0;
      }

      if (cloud_positions[i].y <= -44) {
	cloud_positions[i].y = 240 - 44;
      }
    }

    next_cloud_move += 100;
  }

  // update the icons
  action_icon->update();
  sword_icon->update();

  // update the animation of the cursor
  cursor->update();
}

/**
 * Displays the common part of the selection menu.
 * This function is called by the display() function of
 * the current selection menu phase.
 * @param destination_surface the surface where to display
 */
void SelectionMenuCommon::display(SDL_Surface *destination_surface) {

  // background color
  SDL_FillRect(destination_surface, NULL, background_color);

  // display the clouds
  SDL_Rect position;
  for (int i = 0; i < 16; i++) {

    position = cloud_positions[i];

    SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);

    if (cloud_positions[i].x >= 320 - 80) {
      position.x = cloud_positions[i].x - 320;
      position.y = cloud_positions[i].y;
      SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);

      if (cloud_positions[i].y <= 0) {
	position.x = cloud_positions[i].x - 320;
	position.y = cloud_positions[i].y + 240;
	SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);
      }
    }

    if (cloud_positions[i].y <= 0) {
      position.x = cloud_positions[i].x;
      position.y = cloud_positions[i].y + 240;
      SDL_BlitSurface(img_cloud, NULL, destination_surface, &position);
    }
  }

  // display the background image
  position.x = 37;
  position.y = 38;
  position.w = 246;
  position.h = 165;
  SDL_BlitSurface(img_background, NULL, destination_surface, &position);

  /* no icons to simplify the keys
  // icons
  action_icon->display(destination_surface);
  sword_icon->display(destination_surface);
  */

}
