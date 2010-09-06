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
#include "menus/PauseSubmenuMap.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Dungeon.h"
#include "ResourceManager.h"
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

const Rectangle PauseSubmenuMap::outside_world_minimap_size(0, 0, 225, 388);

/**
 * @brief Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuMap::PauseSubmenuMap(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game),
  equipment(game->get_equipment()), dungeon(game->get_current_dungeon()) {

  // outside map or inside map (no dungeon): show the world map
  if (dungeon == NULL) {

    set_caption_text(StringResource::get_string("map.caption.world_map"));

    const Rectangle &real_size = game->get_outside_world_size();

    Map *map = game->get_current_map();
    hero_position = map->get_location();

    if (map->is_in_outside_world()) {
      const Rectangle &hero_map_xy = game->get_hero_xy();
      hero_position.add_xy(hero_map_xy.get_x(), hero_map_xy.get_y());
    }

    hero_position.set_x(hero_position.get_x() * outside_world_minimap_size.get_width() / real_size.get_width());
    hero_position.set_y(hero_position.get_y() * outside_world_minimap_size.get_height() / real_size.get_height());

    if (equipment->has_ability("see_outside_world_minimap")) {
      world_map_img = new Surface("menus/outside_world_map.png");
      world_minimap_visible_y = std::min(388 - 133, std::max(0, hero_position.get_y() - 66));
    }
    else {
      world_map_img = new Surface("menus/outside_world_clouds.png");
      world_minimap_visible_y = 0;
    }
    moving_visible_y = 0;

    hero_position.add_xy(48 - 8, 59 - 6);

    hero_point_sprite = NULL;
  }

  // dungeon
  else {
    // caption text
    set_caption_text(dungeon->get_name());

    // item icons
    dungeon_map_background = new Surface("menus/dungeon_map_background.png");
    dungeon_map_icons = new Surface("menus/dungeon_map_icons.png");

    small_keys_counter = new Counter(2, false, 124, 182);
    small_keys_counter->set_value(equipment->get_small_keys());

    // floors
    dungeon_floors_img = new Surface("floors.png", Surface::DIR_LANGUAGE);

    hero_floor = game->get_current_map()->get_floor();
    boss_floor = dungeon->get_boss_floor();

    highest_floor = dungeon->get_highest_floor();
    lowest_floor = dungeon->get_lowest_floor();
    nb_floors = dungeon->get_nb_floors();

    nb_floors_displayed = dungeon->get_nb_floors_displayed();
    highest_floor_displayed = dungeon->get_highest_floor_displayed(hero_floor);
    selected_floor = hero_floor;

    // map
    dungeon_map_img = new Surface(123, 119);
    dungeon_map_img->set_transparency_color(Color::get_black());
    load_dungeon_map_image();
  }

  hero_head_sprite = new Sprite("menus/hero_head");
  std::ostringstream oss;
  oss << "tunic" << equipment->get_ability("tunic");
  hero_head_sprite->set_current_animation(oss.str());

  up_arrow_sprite = new Sprite("menus/arrow");
  up_arrow_sprite->set_current_direction(1);

  down_arrow_sprite = new Sprite("menus/arrow");
  down_arrow_sprite->set_current_direction(3);
}

/**
 * @brief Destructor.
 */
PauseSubmenuMap::~PauseSubmenuMap(void) {

  if (dungeon == NULL) {
    delete world_map_img;
  }
  else {
    delete dungeon_map_background;
    delete dungeon_map_icons;
    delete dungeon_floors_img;
    delete small_keys_counter;
    delete dungeon_map_img;
    delete hero_point_sprite;
  }

  delete hero_head_sprite;
  delete up_arrow_sprite;
  delete down_arrow_sprite;
}

/**
 * @brief Converts some coordinates relative to the dungeon floor into coordinates relative to the minimap displayed.
 * @param floor_coords coordinates relative to the floor
 * @param minimap_coords coordinates relative to the minimap (result of the function)
 * @param floor_size size of the floor
 */
void PauseSubmenuMap::to_dungeon_minimap_coordinates(const Rectangle &floor_coords,
    Rectangle &minimap_coords, const Rectangle &floor_size) {

  Rectangle minimap_rectangle(0, 0, 123, 119);

  if ((floor_size.get_width() * 119.0) / (floor_size.get_height() * 123.0) > 1.0) {
    // the floor height does not use the entire vertical space
    minimap_rectangle.set_height(floor_size.get_height() * 123 / floor_size.get_width());
    minimap_rectangle.set_y((119 - minimap_rectangle.get_height()) / 2);
  }
  else {
    // the floor width does not use the entire horizontal space
    minimap_rectangle.set_width(floor_size.get_width() * 119 / floor_size.get_height());
    minimap_rectangle.set_x((123 - minimap_rectangle.get_width()) / 2);
  }

  /*
  std::cout << "minimap_rectangle.x = " << minimap_rectangle.x << ", floor_coords.x = " << floor_coords.x
    << ", minimap_rectangle.w = " << minimap_rectangle.w << ", floor_size.w = " << floor_size.w << std::endl;
    */
  minimap_coords.set_x(minimap_rectangle.get_x() +
      floor_coords.get_x() * minimap_rectangle.get_width() / floor_size.get_width());
  minimap_coords.set_y(minimap_rectangle.get_y() +
      floor_coords.get_y() * minimap_rectangle.get_height() / floor_size.get_height());
}

/**
 * @brief Deletes any previous dungeon map image and loads the image
 * corresponding to the selected floor.
 */
void PauseSubmenuMap::load_dungeon_map_image(void) {

  dungeon_map_img->fill_with_color(Color::get_black());

  if (equipment->has_ability("see_dungeon_minimap_rooms")) {

    // load the image of this floor
    std::ostringstream oss;
    oss << "maps/dungeons/map" << dungeon->get_number() << "_" << selected_floor << ".png";
    Surface *floor_map_img = new Surface(oss.str(), Surface::DIR_DATA);
    floor_map_img->blit(dungeon_map_img);
    delete floor_map_img;
  }

  if (!equipment->has_ability("see_dungeon_minimap_elements")) {
    hero_point_sprite = NULL;
  }

  else {
    // the hero's position
    hero_point_sprite = new Sprite("menus/hero_point");

    const Rectangle &floor_size = dungeon->get_floor_size(selected_floor);

    hero_position = game->get_current_map()->get_location();
    const Rectangle &hero_map_xy = game->get_hero_xy();

    hero_position.add_xy(hero_map_xy.get_x(), hero_map_xy.get_y());

    to_dungeon_minimap_coordinates(hero_position, hero_position, floor_size);

    // chests
    Rectangle small_chest_src_position(78, 8, 4, 4);
    Rectangle big_chest_src_position(78, 12, 6, 4);
    Rectangle dst_position;
    const std::vector<Dungeon::DungeonElement> &chests = dungeon->get_chests(selected_floor);
    for (unsigned int i = 0; i < chests.size(); i++) {

      if (!savegame->get_boolean(chests[i].savegame_variable)) {

	int dx = chests[i].big ? 16 : 8;  // TODO change chests origin point to avoid this
	int dy = chests[i].big ? 21 : 13;

	dst_position.set_xy(chests[i].x + dx, chests[i].y + dy);
	to_dungeon_minimap_coordinates(dst_position, dst_position, floor_size);
	dst_position.add_x((chests[i].big) ? -2 : -1);
	dst_position.add_y(-1);

	if (!chests[i].big) {
	  dungeon_map_icons->blit(small_chest_src_position, dungeon_map_img, dst_position);
	}
	else {
	  dungeon_map_icons->blit(big_chest_src_position, dungeon_map_img, dst_position);
	}
      }
    }

    // boss and minibosses
    Rectangle src_position(78, 0, 8, 8);
    
    const std::vector<Dungeon::DungeonElement> &bosses = dungeon->get_bosses(selected_floor);
    for (unsigned int i = 0; i < bosses.size(); i++) {

      // TODO also display minibosses?
      if (bosses[i].big && !savegame->get_boolean(bosses[i].savegame_variable)) {

	dst_position.set_xy(bosses[i].x, bosses[i].y);
	to_dungeon_minimap_coordinates(dst_position, dst_position, floor_size);
	dst_position.add_xy(-2, -2);

	dungeon_map_icons->blit(src_position, dungeon_map_img, dst_position);
      }
    }
  }
}

/**
 * @brief This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuMap::key_pressed(GameControls::GameKey key) {

  switch (key) {

  case GameControls::LEFT:
    pause_menu->show_left_submenu();
    break;

  case GameControls::RIGHT:
    pause_menu->show_right_submenu();
    break;

  case GameControls::UP:
  case GameControls::DOWN:

    if (dungeon == NULL) {

      // move the world map
      if (equipment->has_ability("see_outside_world_minimap")) {
	moving_visible_y = (key == GameControls::UP) ? -1 : 1;
	next_moving_visible_y_date = System::now();
      }
    }

    else {
      // select another floor
      int new_selected_floor = selected_floor + ((key == GameControls::UP) ? 1 : -1);
      if (new_selected_floor >= lowest_floor && new_selected_floor <= highest_floor) {

	ResourceManager::get_sound("cursor")->play();
	hero_head_sprite->restart_animation();
	selected_floor = new_selected_floor;
	load_dungeon_map_image();

	if (selected_floor <= highest_floor_displayed - 7) {
	  highest_floor_displayed--;
	}
	else if (selected_floor > highest_floor_displayed) {
	  highest_floor_displayed++;
	}
      }
    }
    break;

  default:
    break;
  }
}

/**
 * @brief Updates this submenu.
 */
void PauseSubmenuMap::update(void) {

  hero_head_sprite->update();
  up_arrow_sprite->update();
  down_arrow_sprite->update();

  if (dungeon == NULL) {

    GameControls *controls = game->get_controls();

    bool up = controls->is_key_pressed(GameControls::UP);
    bool down = controls->is_key_pressed(GameControls::DOWN);

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
  }

  if (hero_point_sprite != NULL) {
    hero_point_sprite->update();
  }
}

/**
 * @brief Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display(Surface *destination) {

  PauseSubmenu::display(destination);

  if (dungeon == NULL) {
    display_world_map(destination);
  }
  else {
    display_dungeon_map(destination);
  }
}

/**
 * @brief Displays the world map.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_world_map(Surface *destination) {

  // display the surface
  Rectangle src_position(0, world_minimap_visible_y, 225, 133);
  static Rectangle dst_position(48, 59, 0, 0);

  world_map_img->blit(src_position, destination, dst_position);

  // if the player can see the miniap
  if (equipment->has_ability("see_outside_world_minimap")) {

    // display the hero's position
    int hero_visible_y = hero_position.get_y() - world_minimap_visible_y;
    if (hero_visible_y >= 51 && hero_visible_y <= 133 + 51) {
      hero_head_sprite->display(destination, hero_position.get_x(), hero_visible_y);
    }

    // display the arrows
    if (world_minimap_visible_y > 0) {
      up_arrow_sprite->display(destination, 96, 55);
      up_arrow_sprite->display(destination, 211, 55);
    }

    if (world_minimap_visible_y < 388 - 133) {
      down_arrow_sprite->display(destination, 96, 188);
      down_arrow_sprite->display(destination, 211, 188);
    }
  }
}

/**
 * @brief Displays the dungeon map submenu.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_dungeon_map(Surface *destination) {

  // show the special background
  Rectangle dst_position(48, 59);
  dungeon_map_background->blit(destination, dst_position);

  // show the dungeon items
  display_dungeon_items(destination);

  // show the floors
  display_dungeon_floors(destination);

  // show the map itself
  dst_position.set_xy(143, 66);
  dungeon_map_img->blit(destination, dst_position);

  if (hero_point_sprite != NULL && selected_floor == hero_floor) {
    hero_point_sprite->display(dungeon_map_img, hero_position.get_x(), hero_position.get_y());
  }
}

/**
 * @brief Displays the dungeon items.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_dungeon_items(Surface *destination) {

  // rooms
  if (equipment->has_ability("see_dungeon_minimap_rooms")) {
    Rectangle src_position(0, 0, 17, 17);
    Rectangle dst_position(50, 168);
    dungeon_map_icons->blit(src_position, destination, dst_position);
  }

  // elements
  if (equipment->has_ability("see_dungeon_minimap_elements")) {
    Rectangle src_position(17, 0, 17, 17);
    Rectangle dst_position(69, 168);
    dungeon_map_icons->blit(src_position, destination, dst_position);
  }

  // big key
  if (equipment->has_ability("open_dungeon_big_locks")) {
    Rectangle src_position(34, 0, 17, 17);
    Rectangle dst_position(88, 168);
    dungeon_map_icons->blit(src_position, destination, dst_position);
  }

  // boss key
  if (equipment->has_ability("open_dungeon_boss_lock")) {
    Rectangle src_position(51, 0, 17, 17);
    Rectangle dst_position(107, 168);
    dungeon_map_icons->blit(src_position, destination, dst_position);
  }

  // small keys
  Rectangle src_position(68, 0, 9, 17);
  Rectangle dst_position(126, 168);
  dungeon_map_icons->blit(src_position, destination, dst_position);
  small_keys_counter->display(destination);
}

/**
 * @brief Displays the dungeon floors.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_dungeon_floors(Surface *destination) {

  // display some floors
  int src_y = (15 - highest_floor_displayed) * 12;
  int src_height = nb_floors_displayed * 12 + 1;

  int dst_y = 70 + (7 - nb_floors_displayed) * 6;

  Rectangle src_position(96, src_y, 32, src_height);
  Rectangle dst_position(79, dst_y);
  dungeon_floors_img->blit(src_position, destination, dst_position);

  // display the current floor with other colors
  src_position.set_xy(64, (15 - selected_floor) * 12);
  src_position.set_height(13);

  dst_position.set_y(dst_y + (highest_floor_displayed - selected_floor) * 12);
  dungeon_floors_img->blit(src_position, destination, dst_position);

  // display the hero's icon
  int lowest_floor_displayed = highest_floor_displayed - nb_floors_displayed + 1;
  if (hero_floor >= lowest_floor_displayed && hero_floor <= highest_floor_displayed) {

    int y = dst_y + (highest_floor_displayed - hero_floor) * 12;
    hero_head_sprite->display(destination, 61, y);
  }

  // display the boss icon
  if (equipment->has_ability("see_dungeon_minimap_elements")
      && boss_floor >= lowest_floor_displayed
      && boss_floor <= highest_floor_displayed) {

    int boss_y = dst_y + (highest_floor_displayed - boss_floor) * 12 + 3;

    Rectangle src_position(78, 0, 8, 8);
    Rectangle dst_position(113, boss_y);

    dungeon_map_icons->blit(src_position, destination, dst_position);
  }

  // display the arrows
  if (lowest_floor_displayed > lowest_floor) {
    down_arrow_sprite->display(destination, 89, 151);
  }

  if (highest_floor_displayed < highest_floor) {
    up_arrow_sprite->display(destination, 89, 66);
  }
}

