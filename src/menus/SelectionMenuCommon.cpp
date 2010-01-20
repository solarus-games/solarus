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
#include "hud/ActionIcon.h"
#include "hud/SwordIcon.h"
#include "hud/HeartsView.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/Color.h"
#include "lowlevel/System.h"
#include "StringResource.h"

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
  background_color = Color(104, 144, 240);

  // texts
  text_option1 = new TextSurface(90, 172, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  text_option2 = new TextSurface(198, 172, TextSurface::ALIGN_LEFT, TextSurface::ALIGN_MIDDLE);
  text_title = new TextSurface(160, 54, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  text_title->set_font(TextSurface::FONT_STANDARD);

  // icons (actually not shown any more in the selection menu)
  keys_effect = new KeysEffect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_VALIDATE);
  keys_effect->set_sword_key_enabled(false);
  // action_icon = new ActionIcon(keys_effect, 13, 31);
  // sword_icon = new SwordIcon(keys_effect, NULL, 0, 9);

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
  delete img_cloud;
  delete img_background;
  delete img_save_container;
  delete img_option_container;

  delete text_option1;
  delete text_option2;
  delete text_title;

  for (int i = 0; i < 3; i++) {
    delete img_numbers[i];
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

  next_cloud_move = System::now();

  int i;

  for (i = 0; i < 16; i++) {
    cloud_positions[i].set_size(80, 44);
  }

  i = 0;

  cloud_positions[i++].set_xy(20, 40);
  cloud_positions[i++].set_xy(50, 160);
  cloud_positions[i++].set_xy(160, 30);
  cloud_positions[i++].set_xy(270, 200);
  cloud_positions[i++].set_xy(200, 120);
  cloud_positions[i++].set_xy(90, 120);
  cloud_positions[i++].set_xy(300, 100);
  cloud_positions[i++].set_xy(240, 10);
  cloud_positions[i++].set_xy(60, 190);
  cloud_positions[i++].set_xy(150, 210);
  cloud_positions[i++].set_xy(310, 220);
  cloud_positions[i++].set_xy(70, 20);
  cloud_positions[i++].set_xy(130, 180);
  cloud_positions[i++].set_xy(200, 700);
  cloud_positions[i++].set_xy(20, 120);
  cloud_positions[i++].set_xy(170, 220);
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
      player_name = (std::string) "- " + StringResource::get_string("selection_menu.empty") + " -";
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
      hearts_views[i] = NULL;
    }

    if (!savegames[i]->is_empty()) {
      hearts_views[i] = new HeartsView(savegames[i]->get_equipment(), 168, 78 + i * 27);
    }
  }
}

/**
 * Updates the common part of the selection menu.
 * This function is called by the update() function of
 * the current selection menu phase.
 */
void SelectionMenuCommon::update(void) {

  // move the clouds
  uint32_t now = System::now();
  while (now >= next_cloud_move) {
    
    for (int i = 0; i < 16; i++) {
      cloud_positions[i].add_x(1);
      cloud_positions[i].add_y(-1);

      if (cloud_positions[i].get_x() >= 320) {
	cloud_positions[i].set_x(0);
      }

      if (cloud_positions[i].get_y() <= -44) {
	cloud_positions[i].set_y(240 - 44);
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
void SelectionMenuCommon::display(Surface *destination_surface) {

  // background color
  destination_surface->fill_with_color(background_color);

  // display the clouds
  Rectangle position;
  for (int i = 0; i < 16; i++) {

    position = cloud_positions[i];

    img_cloud->blit(destination_surface, position);

    if (cloud_positions[i].get_x() >= 320 - 80) {
      position.set_xy(cloud_positions[i].get_x() - 320, cloud_positions[i].get_y());
      img_cloud->blit(destination_surface, position);

      if (cloud_positions[i].get_y() <= 0) {
	position.set_xy(cloud_positions[i].get_x() - 320, cloud_positions[i].get_y() + 240);
	img_cloud->blit(destination_surface, position);
      }
    }

    if (cloud_positions[i].get_y() <= 0) {
      position.set_xy(cloud_positions[i].get_x(), cloud_positions[i].get_y() + 240);
      img_cloud->blit(destination_surface, position);
    }
  }

  // display the background image
  position.set_xy(37, 38);
  position.set_size(246, 165);
  img_background->blit(destination_surface, position);

  /* no icons to simplify the keys
  // icons
  action_icon->display(destination_surface);
  sword_icon->display(destination_surface);
  */

}

