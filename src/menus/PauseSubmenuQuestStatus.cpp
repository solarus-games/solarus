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
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "StringResource.h"
#include "lowlevel/Color.h"
#include "lowlevel/Sound.h"
#include <sstream>

/**
 * @brief Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuQuestStatus::PauseSubmenuQuestStatus(PauseMenu& pause_menu, Game& game):
  PauseSubmenu(pause_menu, game),
  quest_items_surface(SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT),
  cursor_sprite("menus/pause_cursor") {

  // TODO all of this is quest-specific and should be scripted

  // draw the items on a surface
  quest_items_surface.set_transparency_color(Color::get_black());
  quest_items_surface.fill_with_color(Color::get_black());

  Surface treasures_img("hud/dialog_icons.png"); // TODO use sprite entities/items
  Surface pieces_of_heart_img("menus/quest_status_pieces_of_heart.png");
  Surface dungeons_img("menus/quest_status_dungeons.png");

  std::ostringstream oss;

  // tunic
  {
    int tunic = equipment.get_ability("tunic");
    Rectangle src_position((tunic - 1) * 16, 96, 16, 16);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE + 17,
        SOLARUS_SCREEN_HEIGHT_MIDDLE + 44);
    treasures_img.blit(src_position, &quest_items_surface, dst_position);

    oss << "quest_status.caption.tunic_" << tunic;
    caption_texts[5] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // sword
  if (equipment.has_ability("sword")) {
    int sword = equipment.get_ability("sword");
    Rectangle src_position(80 + sword * 16, 96, 16, 16);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE + 51,
        SOLARUS_SCREEN_HEIGHT_MIDDLE + 44);
    treasures_img.blit(src_position, &quest_items_surface, dst_position);

    oss << "quest_status.caption.sword_" << sword;
    caption_texts[6] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // shield
  if (equipment.has_ability("shield")) {
    int shield = equipment.get_ability("shield");
    Rectangle src_position(32 + shield * 16, 96, 16, 16);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE + 85,
        SOLARUS_SCREEN_HEIGHT_MIDDLE + 44);
    treasures_img.blit(src_position, &quest_items_surface, dst_position);

    oss << "quest_status.caption.shield_" << shield;
    caption_texts[7] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // rupee bag
  {
    int rupee_bag = equipment.get_item_variant("rupee_bag");
    
    Rectangle src_position(rupee_bag * 16, 80, 16, 16);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE - 100,
        SOLARUS_SCREEN_HEIGHT_MIDDLE - 49);
    treasures_img.blit(src_position, &quest_items_surface, dst_position);

    oss << "quest_status.caption.rupee_bag_" << rupee_bag;
    caption_texts[0] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // bomb bag
  int bomb_bag = equipment.get_item_variant("bomb_bag");
  if (bomb_bag != 0) {
    Rectangle src_position(48 + bomb_bag * 16, 80, 16, 16);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE - 100,
        SOLARUS_SCREEN_HEIGHT_MIDDLE - 20);
    treasures_img.blit(src_position, &quest_items_surface, dst_position);

    oss << "quest_status.caption.bomb_bag_" << bomb_bag;
    caption_texts[1] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // quiver
  int quiver = equipment.get_item_variant("quiver");
  if (quiver != 0) {
    
    Rectangle src_position(96 + quiver * 16, 80, 16, 16);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE - 100,
        SOLARUS_SCREEN_HEIGHT_MIDDLE + 10);
    treasures_img.blit(src_position, &quest_items_surface, dst_position);

    oss << "quest_status.caption.quiver_" << quiver;
    caption_texts[2] = StringResource::get_string(oss.str());
    oss.str("");
  }

  // world map
  if (equipment.has_item("world_map")) {
    Rectangle src_position(0, 80, 16, 16);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE - 100,
        SOLARUS_SCREEN_HEIGHT_MIDDLE + 44);
    treasures_img.blit(src_position, &quest_items_surface, dst_position);

    caption_texts[3] = StringResource::get_string("quest_status.caption.world_map");
  }

  // pieces of heart
  {
    int x = 51 * savegame.get_integer(1030);
    Rectangle src_position(x, 0, 51, 50);
    Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE - 59,
        SOLARUS_SCREEN_HEIGHT_MIDDLE -38);
    pieces_of_heart_img.blit(src_position, &quest_items_surface, dst_position);

    caption_texts[4] = StringResource::get_string("quest_status.caption.pieces_of_heart");
  }

  // dungeons finished
  {
    static const Rectangle dst_positions[] = {
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 49,
            SOLARUS_SCREEN_HEIGHT_MIDDLE - 51),
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 72,
            SOLARUS_SCREEN_HEIGHT_MIDDLE - 46),
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 83,
            SOLARUS_SCREEN_HEIGHT_MIDDLE - 23),
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 72,
            SOLARUS_SCREEN_HEIGHT_MIDDLE),
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 49,
            SOLARUS_SCREEN_HEIGHT_MIDDLE + 7),
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 26,
            SOLARUS_SCREEN_HEIGHT_MIDDLE),
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 15,
            SOLARUS_SCREEN_HEIGHT_MIDDLE - 23),
        Rectangle(SOLARUS_SCREEN_WIDTH_MIDDLE + 26,
            SOLARUS_SCREEN_HEIGHT_MIDDLE - 46)
    };
    for (int i = 0; i < 8; i++) {
      if (equipment.is_dungeon_finished(i + 1)) {
        Rectangle src_position(i * 20, 0, 20, 20);
        dungeons_img.blit(src_position, &quest_items_surface, dst_positions[i]);
      }
    }
  }

  // cursor
  cursor_position = -1;
  set_cursor_position(0);
}

/**
 * @brief Destructor.
 */
PauseSubmenuQuestStatus::~PauseSubmenuQuestStatus() {

}

/**
 * @brief Changes the position of the cursor.
 * @param position new cursor position, from 0 to 7
 */
void PauseSubmenuQuestStatus::set_cursor_position(int position) {

  if (position != this->cursor_position) {
    this->cursor_position = position;

    if (position <= 3) {
      cursor_sprite_position.set_x(SOLARUS_SCREEN_WIDTH_MIDDLE-  92);
    }
    else if (position == 4) {
      cursor_sprite_position.set_x(SOLARUS_SCREEN_WIDTH_MIDDLE - 34);
    }
    else {
      cursor_sprite_position.set_x(
          SOLARUS_SCREEN_WIDTH_MIDDLE - 141 + 34 * position - 4);
    }

    switch (position) {

      case 0:
        cursor_sprite_position.set_y(SOLARUS_SCREEN_HEIGHT_MIDDLE - 41);
        break;

      case 1:
        cursor_sprite_position.set_y(SOLARUS_SCREEN_HEIGHT_MIDDLE - 12);
        break;

      case 2:
        cursor_sprite_position.set_y(SOLARUS_SCREEN_HEIGHT_MIDDLE + 18);
        break;

      case 4:
        cursor_sprite_position.set_y(SOLARUS_SCREEN_HEIGHT_MIDDLE - 13);
        break;

      default:
        cursor_sprite_position.set_y(SOLARUS_SCREEN_HEIGHT_MIDDLE + 52);
        break;
    }

    set_caption_text(caption_texts[position]);
  }
}

/**
 * @brief This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuQuestStatus::key_pressed(GameControls::GameKey key) {

  switch (key) {

    case GameControls::LEFT:

      if (cursor_position <= 3) {
        pause_menu.show_left_submenu();
      }
      else {
        Sound::play("cursor");

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

    case GameControls::RIGHT:

      if (cursor_position == 4 || cursor_position == 7) {
        pause_menu.show_right_submenu();
      }
      else {
        Sound::play("cursor");

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

    case GameControls::DOWN:
      Sound::play("cursor");
      set_cursor_position((cursor_position + 1) % 8);
      break;

    case GameControls::UP:
      Sound::play("cursor");
      set_cursor_position((cursor_position + 7) % 8);
      break;

    default:
      break;
  }
}

/**
 * @brief Updates this submenu.
 */
void PauseSubmenuQuestStatus::update() {
  cursor_sprite.update();
}

/**
 * @brief Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuQuestStatus::display(Surface* destination) {
  PauseSubmenu::display(destination);

  // quest items
  quest_items_surface.blit(destination);

  // cursor
  cursor_sprite.display(destination,
      cursor_sprite_position.get_x(), cursor_sprite_position.get_y());
}

