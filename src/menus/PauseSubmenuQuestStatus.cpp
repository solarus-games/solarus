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
#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Color.h"
#include "Sound.h"
#include "Sprite.h"
#include "StringResource.h"
using namespace std;

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuQuestStatus::PauseSubmenuQuestStatus(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {

  Equipment *equipment = game->get_equipment();

  // draw the items on a surface
  quest_items_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  SDL_SetColorKey(quest_items_surface, SDL_SRCCOLORKEY, Color::black);
  SDL_FillRect(quest_items_surface, NULL, Color::black);

  SDL_Surface *treasures_img = ResourceManager::load_image("hud/message_and_treasure_icons.png");
  SDL_Surface *pieces_of_heart_img = ResourceManager::load_image("menus/quest_status_pieces_of_heart.png");

  ostringstream oss;

  // tunic
  {
    int tunic = equipment->get_tunic();
    SDL_Rect src_position = {tunic * 16, 96, 16, 16};
    SDL_Rect dst_position = {177, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    oss << "quest_status.caption.tunic_" << tunic;
    caption_texts[5] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // sword
  if (equipment->has_sword()) {
    int sword = equipment->get_sword();
    SDL_Rect src_position = {80 + sword * 16, 96, 16, 16};
    SDL_Rect dst_position = {211, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    oss << "quest_status.caption.sword_" << sword;
    caption_texts[6] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // shield
  if (equipment->has_shield()) {
    int shield = equipment->get_shield();
    SDL_Rect src_position = {32 + shield * 16, 96, 16, 16};
    SDL_Rect dst_position = {245, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    oss << "quest_status.caption.shield_" << shield;
    caption_texts[7] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // rupee bag
  {
    int max_rupees = equipment->get_max_rupees();
    int rupee_bag = (max_rupees == 100) ? 1 : ((max_rupees == 300) ? 2 : 3);
    
    SDL_Rect src_position = {rupee_bag * 16, 80, 16, 16};
    SDL_Rect dst_position = {60, 71};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    oss << "quest_status.caption.rupee_bag_" << rupee_bag;
    caption_texts[0] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // bomb bag
  int max_bombs = equipment->get_max_bombs();
  if (max_bombs != 0) {
    int bomb_bag = (max_bombs == 10) ? 1 : ((max_bombs == 30) ? 2 : 3);
    SDL_Rect src_position = {48 + bomb_bag * 16, 80, 16, 16};
    SDL_Rect dst_position = {60, 100};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    oss << "quest_status.caption.bomb_bag_" << bomb_bag;
    caption_texts[1] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // quiver
  int max_arrows = equipment->get_max_arrows();
  if (max_arrows != 0) {
    int quiver = (max_arrows == 10) ? 1 : ((max_arrows == 30) ? 2 : 3);
    
    SDL_Rect src_position = {96 + quiver * 16, 80, 16, 16};
    SDL_Rect dst_position = {60, 130};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    oss << "quest_status.caption.quiver_" << quiver;
    caption_texts[2] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // world map
  if (equipment->has_world_map()) {
    SDL_Rect src_position = {0, 80, 16, 16};
    SDL_Rect dst_position = {60, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[3] = StringResource::get_string("quest_status.caption.world_map");
  }

  // heart pieces
  {
    int x = 51 * equipment->get_nb_pieces_of_heart();
    SDL_Rect src_position = {x, 0, 51, 50};
    SDL_Rect dst_position = {101, 82};
    SDL_BlitSurface(pieces_of_heart_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[4] = StringResource::get_string("quest_status.caption.pieces_of_heart");
  }

  // dungeons finished
  // TODO

  // cursor
  cursor_sprite = new Sprite("menus/pause_cursor");
  cursor_position = -1;
  set_cursor_position(0);

  SDL_FreeSurface(treasures_img);
  SDL_FreeSurface(pieces_of_heart_img);
}

/**
 * Destructor.
 */
PauseSubmenuQuestStatus::~PauseSubmenuQuestStatus(void) {

  delete cursor_sprite;
  SDL_FreeSurface(quest_items_surface);
}

/**
 * Changes the position of the cursor.
 * @param position new cursor position, from 0 to 7
 */
void PauseSubmenuQuestStatus::set_cursor_position(int position) {

  if (position != this->cursor_position) {
    this->cursor_position = position;

    if (position <= 3) {
      cursor_sprite_position.x = 68;
    }
    else if (position == 4) {
      cursor_sprite_position.x = 126;
    }
    else {
      cursor_sprite_position.x = 19 + 34 * position - 4;
    }

    switch (position) {

    case 0:
      cursor_sprite_position.y = 79;
      break;

    case 1:
      cursor_sprite_position.y = 108;
      break;

    case 2:
      cursor_sprite_position.y = 138;
      break;

    case 4:
      cursor_sprite_position.y = 107;
      break;

    default:
      cursor_sprite_position.y = 172;
      break;
    }

    set_caption_text(caption_texts[position]);
  }
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuQuestStatus::key_pressed(Controls::GameKey key) {

  switch (key) {

  case Controls::LEFT:

    if (cursor_position <= 3) {
      pause_menu->show_left_submenu();
    }
    else {
      ResourceManager::get_sound("cursor")->play();

      if (cursor_position == 4) {
	set_cursor_position(0);
      }
      else if (cursor_position == 5) {
	set_cursor_position(3);
      }
      else {
	set_cursor_position(cursor_position - 1);
      }
    }
    break;

  case Controls::RIGHT:

    if (cursor_position == 4 || cursor_position == 7) {
      pause_menu->show_right_submenu();
    }
    else {
      ResourceManager::get_sound("cursor")->play();

      if (cursor_position <= 2) {
	set_cursor_position(4);
      }
      else if (cursor_position == 3) {
	set_cursor_position(5);
      }
      else {
	set_cursor_position(cursor_position + 1);
      }
    }
    break;

  case Controls::DOWN:
    ResourceManager::get_sound("cursor")->play();
    set_cursor_position((cursor_position + 1) % 8);
    break;

  case Controls::UP:
    ResourceManager::get_sound("cursor")->play();
    set_cursor_position((cursor_position + 7) % 8);
    break;

  default:
    break;
  }
}

/**
 * Updates this submenu.
 */
void PauseSubmenuQuestStatus::update(void) {
  cursor_sprite->update();
}

/**
 * Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuQuestStatus::display(SDL_Surface *destination) {
  PauseSubmenu::display(destination);

  // quest items
  SDL_BlitSurface(quest_items_surface, NULL, destination, NULL);

  // cursor
  cursor_sprite->display(destination, cursor_sprite_position.x, cursor_sprite_position.y);
}

