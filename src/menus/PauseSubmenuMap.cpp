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
#include "menus/PauseSubmenuMap.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Sprite.h"
#include "Map.h"
#include "Equipment.h"
#include "Counter.h"
#include "Savegame.h"
#include "StringResource.h"
#include "lowlevel/Color.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"
#include <sstream>

const Rectangle PauseSubmenuMap::outside_world_minimap_size(0, 0, 225, 388);

/**
 * @brief Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuMap::PauseSubmenuMap(PauseMenu& pause_menu, Game& game):
  PauseSubmenu(pause_menu, game) {

  // outside map or inside map (no dungeon): show the world map
//  if (!game.is_in_dungeon()) {

    set_caption_text(StringResource::get_string("map.caption.world_map"));

    const Rectangle real_size(0, 0, 2080, 3584);

    Map& map = game.get_current_map();
    hero_position = map.get_location();

    if (map.is_in_outside_world()) {
      const Rectangle& hero_map_xy = game.get_hero_xy();
      hero_position.add_xy(hero_map_xy.get_x(), hero_map_xy.get_y());
    }

    hero_position.set_x(hero_position.get_x() * outside_world_minimap_size.get_width() / real_size.get_width());
    hero_position.set_y(hero_position.get_y() * outside_world_minimap_size.get_height() / real_size.get_height());

    if (equipment.has_ability("see_outside_world_minimap")) {
      world_map_img = new Surface("menus/outside_world_map.png");
      world_minimap_visible_y = std::min(388 - 133, std::max(0, hero_position.get_y() - 66));
    }
    else {
      world_map_img = new Surface("menus/outside_world_clouds.png");
      world_minimap_visible_y = 0;
    }
    moving_visible_y = 0;
    hero_position.add_xy(SOLARUS_SCREEN_WIDTH_MIDDLE - 120,
        SOLARUS_SCREEN_HEIGHT_MIDDLE - 67);
    hero_point_sprite = NULL;

    if (equipment.has_ability("see_outside_world_minimap")) {
      equipment.notify_ability_used("see_outside_world_minimap");
    }
//  }

  hero_head_sprite = new Sprite("menus/hero_head");
  std::ostringstream oss;
  oss << "tunic" << equipment.get_ability("tunic");
  hero_head_sprite->set_current_animation(oss.str());

  up_arrow_sprite = new Sprite("menus/arrow");
  up_arrow_sprite->set_current_direction(1);

  down_arrow_sprite = new Sprite("menus/arrow");
  down_arrow_sprite->set_current_direction(3);
}

/**
 * @brief Destructor.
 */
PauseSubmenuMap::~PauseSubmenuMap() {

  delete world_map_img;
  delete hero_head_sprite;
  delete up_arrow_sprite;
  delete down_arrow_sprite;
}

/**
 * @brief This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuMap::key_pressed(GameCommands::GameCommand key) {

  switch (key) {

  case GameCommands::LEFT:
    pause_menu.show_left_submenu();
    break;

  case GameCommands::RIGHT:
    pause_menu.show_right_submenu();
    break;

  case GameCommands::UP:
  case GameCommands::DOWN:

    // move the world map
    if (equipment.has_ability("see_outside_world_minimap")) {
      moving_visible_y = (key == GameCommands::UP) ? -1 : 1;
      next_moving_visible_y_date = System::now();
    }
    break;

  default:
    break;
  }
}

/**
 * @brief Updates this submenu.
 */
void PauseSubmenuMap::update() {

  hero_head_sprite->update();
  up_arrow_sprite->update();
  down_arrow_sprite->update();

  GameCommands &controls = game.get_controls();

  bool up = controls.is_key_pressed(GameCommands::UP);
  bool down = controls.is_key_pressed(GameCommands::DOWN);

  if (moving_visible_y == -1) {

    if (!up) {
      moving_visible_y = down ? 1 : 0;
    }
    else if (world_minimap_visible_y <= 0) {
      moving_visible_y = 0;
    }
  }
  else if (moving_visible_y == 1) {

    if (!down) {
      moving_visible_y = up ? -1 : 0;
    }
    else if (world_minimap_visible_y >= 388 - 133) {
      moving_visible_y = 0;
    }
  }

  uint32_t now = System::now();
  if (moving_visible_y != 0 && now >= next_moving_visible_y_date) {
    world_minimap_visible_y += moving_visible_y;
    next_moving_visible_y_date += 10;
  }

  if (hero_point_sprite != NULL) {
    hero_point_sprite->update();
  }
}

/**
 * @brief Draws this submenu.
 * @param dst_surface the destination surface
 */
void PauseSubmenuMap::draw(Surface& dst_surface) {

  PauseSubmenu::draw(dst_surface);

  draw_world_map(dst_surface);
}

/**
 * @brief Draws the world map.
 * @param dst_surface the destination surface
 */
void PauseSubmenuMap::draw_world_map(Surface& dst_surface) {

  // draw the surface
  Rectangle src_position(0, world_minimap_visible_y, 225, 133);
  static Rectangle dst_position(SOLARUS_SCREEN_WIDTH_MIDDLE - 112,
      SOLARUS_SCREEN_HEIGHT_MIDDLE - 61, 0, 0);

  world_map_img->draw_region(src_position, dst_surface, dst_position);

  // if the player can see the minimap
  if (equipment.has_ability("see_outside_world_minimap")) {

    // draw the hero's position
    int hero_visible_y = hero_position.get_y() - world_minimap_visible_y;
    if (hero_visible_y >= 51 && hero_visible_y <= 133 + 51) {
      hero_head_sprite->draw(dst_surface, hero_position.get_x(), hero_visible_y);
    }

    // draw the arrows
    if (world_minimap_visible_y > 0) {
      up_arrow_sprite->draw(dst_surface,
          SOLARUS_SCREEN_WIDTH_MIDDLE - 64, SOLARUS_SCREEN_HEIGHT_MIDDLE - 65);
      up_arrow_sprite->draw(dst_surface,
          SOLARUS_SCREEN_WIDTH_MIDDLE + 51, SOLARUS_SCREEN_HEIGHT_MIDDLE - 65);
    }

    if (world_minimap_visible_y < 388 - 133) {
      down_arrow_sprite->draw(dst_surface,
          SOLARUS_SCREEN_WIDTH_MIDDLE - 64, SOLARUS_SCREEN_HEIGHT_MIDDLE + 68);
      down_arrow_sprite->draw(dst_surface,
          SOLARUS_SCREEN_WIDTH_MIDDLE + 51, SOLARUS_SCREEN_HEIGHT_MIDDLE + 68);
    }
  }
}

