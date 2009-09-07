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
#include "menus/PauseSubmenuMap.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Dungeon.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "Sound.h"
#include "Map.h"
#include "Equipment.h"
#include "DungeonEquipment.h"
#include "Counter.h"
#include "Color.h"
#include "Controls.h"
#include "Savegame.h"
#include "StringResource.h"

const SDL_Rect PauseSubmenuMap::outside_world_minimap_size = {0, 0, 225, 388};

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuMap::PauseSubmenuMap(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game),
  equipment(game->get_equipment()), dungeon_equipment(game->get_dungeon_equipment()),
  dungeon(game->get_current_dungeon()) {

  // outside map or inside map (no dungeon): show the world map
  if (dungeon == NULL) {

    set_caption_text(StringResource::get_string("map.caption.world_map"));

    const SDL_Rect *real_size = game->get_outside_world_size();

    Map *map = game->get_current_map();
    hero_position = map->get_location();

    if (map->is_in_outside_world()) {
      const SDL_Rect &hero_map_xy = game->get_hero_xy();
      hero_position.x += hero_map_xy.x;
      hero_position.y += hero_map_xy.y;
    }

    hero_position.x = hero_position.x * outside_world_minimap_size.w / real_size->w;
    hero_position.y = hero_position.y * outside_world_minimap_size.h / real_size->h;

    if (equipment->has_world_map()) {
      world_map_img = ResourceManager::load_image("menus/outside_world_map.png");
      world_minimap_visible_y = MIN(388 - 133, MAX(0, hero_position.y - 66));
    }
    else {
      world_map_img = ResourceManager::load_image("menus/outside_world_clouds.png");
      world_minimap_visible_y = 0;
    }
    moving_visible_y = 0;

    hero_position.x += 48 - 8;
    hero_position.y += 59 - 6;

    hero_point_sprite = NULL;
  }

  // dungeon
  else {
    // caption text
    set_caption_text(dungeon->get_name());

    // item icons
    dungeon_map_background = ResourceManager::load_image("menus/dungeon_map_background.png");
    dungeon_map_icons = ResourceManager::load_image("menus/dungeon_map_icons.png");

    small_keys_counter = new Counter(2, false, 124, 182);
    small_keys_counter->set_value(equipment->get_small_keys());

    // floors
    dungeon_floors_img = ResourceManager::load_image("hud/floors.png");

    hero_floor = game->get_current_map()->get_floor();
    boss_floor = dungeon->get_boss_floor();

    highest_floor = dungeon->get_highest_floor();
    lowest_floor = dungeon->get_lowest_floor();
    nb_floors = dungeon->get_nb_floors();

    nb_floors_displayed = dungeon->get_nb_floors_displayed();
    highest_floor_displayed = dungeon->get_highest_floor_displayed(hero_floor);
    selected_floor = hero_floor;

    // map
    dungeon_map_img = SDL_CreateRGBSurface(SDL_HWSURFACE, 123, 119, 32, 0, 0, 0, 0);
    SDL_SetColorKey(dungeon_map_img, SDL_SRCCOLORKEY, Color::black);
    load_dungeon_map_image();
  }

  hero_head_sprite = new Sprite("menus/hero_head");
  std::ostringstream oss;
  oss << "tunic" << equipment->get_tunic();
  hero_head_sprite->set_current_animation(oss.str());

  up_arrow_sprite = new Sprite("menus/arrow");
  up_arrow_sprite->set_current_direction(0);

  down_arrow_sprite = new Sprite("menus/arrow");
  down_arrow_sprite->set_current_direction(1);
}

/**
 * Destructor.
 */
PauseSubmenuMap::~PauseSubmenuMap(void) {

  if (dungeon == NULL) {
    SDL_FreeSurface(world_map_img);
  }
  else {
    SDL_FreeSurface(dungeon_map_background);
    SDL_FreeSurface(dungeon_map_icons);
    SDL_FreeSurface(dungeon_floors_img);
    delete small_keys_counter;

    SDL_FreeSurface(dungeon_map_img);

    if (hero_point_sprite != NULL) {
      delete hero_point_sprite;
    }
  }

  delete hero_head_sprite;
  delete up_arrow_sprite;
  delete down_arrow_sprite;
}

/**
 * Converts some coordinates relative to the dungeon floor into coordinates relative to the minimap displayed.
 * @param floor_coords coordinates relative to the floor
 * @param minimap_coords coordinates relative to the minimap (result of the function)
 * @param floor_size size of the floor
 */
void PauseSubmenuMap::to_dungeon_minimap_coordinates(const SDL_Rect &floor_coords, SDL_Rect &minimap_coords, const SDL_Rect &floor_size) {

  SDL_Rect minimap_rectangle = { 0, 0, 123, 119 };

  if ((floor_size.w * 119.0) / (floor_size.h * 123.0) > 1.0) {
    // the floor height does not use the entire vertical space
    minimap_rectangle.h = floor_size.h * 123 / floor_size.w;
    minimap_rectangle.y = (119 - minimap_rectangle.h) / 2;
  }
  else {
    // the floor width does not use the entire horizontal space
    minimap_rectangle.w = floor_size.w * 119 / floor_size.h;
    minimap_rectangle.x = (123 - minimap_rectangle.w) / 2;
  }

  /*
  std::cout << "minimap_rectangle.x = " << minimap_rectangle.x << ", floor_coords.x = " << floor_coords.x
    << ", minimap_rectangle.w = " << minimap_rectangle.w << ", floor_size.w = " << floor_size.w << std::endl;
    */
  minimap_coords.x = minimap_rectangle.x + floor_coords.x * minimap_rectangle.w / floor_size.w;
  minimap_coords.y = minimap_rectangle.y + floor_coords.y * minimap_rectangle.h / floor_size.h;
}

/**
 * Deletes any previous dungeon map image and loads the image
 * corresponding to the selected floor.
 */
void PauseSubmenuMap::load_dungeon_map_image(void) {

  SDL_FillRect(dungeon_map_img, NULL, Color::black);

  if (dungeon_equipment->has_map()) {

    // load the image of this floor
    std::ostringstream oss;
    oss << "maps/dungeons/map" << dungeon->get_number() << "_" << selected_floor << ".png";
    SDL_Surface *floor_map_img = ResourceManager::load_image(oss.str(), false);
    SDL_BlitSurface(floor_map_img, NULL, dungeon_map_img, NULL);
    SDL_FreeSurface(floor_map_img);
  }

  if (!dungeon_equipment->has_compass()) {
    hero_point_sprite = NULL;
  }

  else {
    // the hero's position
    hero_point_sprite = new Sprite("menus/hero_point");

    const SDL_Rect &floor_size = dungeon->get_floor_size(selected_floor);

    hero_position = game->get_current_map()->get_location();
    const SDL_Rect &hero_map_xy = game->get_hero_xy();

    hero_position.x += hero_map_xy.x;
    hero_position.y += hero_map_xy.y;

    to_dungeon_minimap_coordinates(hero_position, hero_position, floor_size);

    // chests
    SDL_Rect small_chest_src_position = {78, 8, 4, 4};
    SDL_Rect big_chest_src_position = {78, 12, 6, 4};
    SDL_Rect dst_position;
    const std::vector<Dungeon::DungeonElement> &chests = dungeon->get_chests(selected_floor);
    for (unsigned int i = 0; i < chests.size(); i++) {

      if (!savegame->get_boolean(chests[i].savegame_variable)) {

	int dx = chests[i].big ? 16 : 8;  // TODO change chests origin point to avoid this
	int dy = chests[i].big ? 21 : 13;

	dst_position.x = chests[i].x + dx;
	dst_position.y = chests[i].y + dy;
	to_dungeon_minimap_coordinates(dst_position, dst_position, floor_size);
	dst_position.x -= (chests[i].big) ? 2 : 1;
	dst_position.y -= 1;

	if (!chests[i].big) {
	  SDL_BlitSurface(dungeon_map_icons, &small_chest_src_position, dungeon_map_img, &dst_position);
	}
	else {
	  SDL_BlitSurface(dungeon_map_icons, &big_chest_src_position, dungeon_map_img, &dst_position);
	}
      }
    }

    // boss and minibosses
    SDL_Rect src_position = {78, 0, 8, 8};
    
    const std::vector<Dungeon::DungeonElement> &bosses = dungeon->get_bosses(selected_floor);
    for (unsigned int i = 0; i < bosses.size(); i++) {

      // TODO also display minibosses?
      if (bosses[i].big && !savegame->get_boolean(bosses[i].savegame_variable)) {

	dst_position.x = bosses[i].x;
	dst_position.y = bosses[i].y;
	to_dungeon_minimap_coordinates(dst_position, dst_position, floor_size);
	dst_position.x -= 2;
	dst_position.y -= 2;

	SDL_BlitSurface(dungeon_map_icons, &src_position, dungeon_map_img, &dst_position);
      }
    }
  }
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuMap::key_pressed(Controls::GameKey key) {

  switch (key) {

  case Controls::LEFT:
    pause_menu->show_left_submenu();
    break;

  case Controls::RIGHT:
    pause_menu->show_right_submenu();
    break;

  case Controls::UP:
  case Controls::DOWN:

    if (dungeon == NULL) {

      // move the world map
      if (equipment->has_world_map()) {
	moving_visible_y = (key == Controls::UP) ? -1 : 1;
	next_moving_visible_y_date = SDL_GetTicks();
      }
    }

    else {
      // select another floor
      int new_selected_floor = selected_floor + ((key == Controls::UP) ? 1 : -1);
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
 * Updates this submenu.
 */
void PauseSubmenuMap::update(void) {

  hero_head_sprite->update();
  up_arrow_sprite->update();
  down_arrow_sprite->update();

  if (dungeon == NULL) {

    Controls *controls = game->get_controls();

    bool up = controls->is_key_pressed(Controls::UP);
    bool down = controls->is_key_pressed(Controls::DOWN);

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

    Uint32 now = SDL_GetTicks();
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
 * Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display(SDL_Surface *destination) {

  PauseSubmenu::display(destination);

  if (dungeon == NULL) {
    display_world_map(destination);
  }
  else {
    display_dungeon_map(destination);
  }
}

/**
 * Displays the world map.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_world_map(SDL_Surface *destination) {

  // display the surface
  SDL_Rect src_position = {0, world_minimap_visible_y, 225, 133};
  static SDL_Rect dst_position = {48, 59, 0, 0};

  SDL_BlitSurface(world_map_img, &src_position, destination, &dst_position);

  // if the player has the map
  if (equipment->has_world_map()) {

    // display the hero's position
    int hero_visible_y = hero_position.y - world_minimap_visible_y;
    if (hero_visible_y >= 51 && hero_visible_y <= 184) {
      hero_head_sprite->display(destination, hero_position.x, hero_position.y - world_minimap_visible_y);
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
 * Displays the dungeon map submenu.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_dungeon_map(SDL_Surface *destination) {

  // show the special background
  SDL_Rect dst_position = {48, 59};
  SDL_BlitSurface(dungeon_map_background, NULL, destination, &dst_position);

  // show the dungeon items
  display_dungeon_items(destination);

  // show the floors
  display_dungeon_floors(destination);

  // show the map itself
  dst_position.x = 143;
  dst_position.y = 66;
  SDL_BlitSurface(dungeon_map_img, NULL, destination, &dst_position);

  if (hero_point_sprite != NULL && selected_floor == hero_floor) {
    hero_point_sprite->display(dungeon_map_img, hero_position.x, hero_position.y);
  }
}

/**
 * Displays the dungeon items.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_dungeon_items(SDL_Surface *destination) {

  // map
  if (dungeon_equipment->has_map()) {
    SDL_Rect src_position = {0, 0, 17, 17};
    SDL_Rect dst_position = {50, 168};
    SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  }

  // compass
  if (dungeon_equipment->has_compass()) {
    SDL_Rect src_position = {17, 0, 17, 17};
    SDL_Rect dst_position = {69, 168};
    SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  }

  // big key
  if (dungeon_equipment->has_big_key()) {
    SDL_Rect src_position = {34, 0, 17, 17};
    SDL_Rect dst_position = {88, 168};
    SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  }

  // boss key
  if (dungeon_equipment->has_boss_key()) {
    SDL_Rect src_position = {51, 0, 17, 17};
    SDL_Rect dst_position = {107, 168};
    SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  }

  // small keys
  SDL_Rect src_position = {68, 0, 9, 17};
  SDL_Rect dst_position = {126, 168};
  SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  small_keys_counter->display(destination);
}

/**
 * Displays the dungeon floors.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_dungeon_floors(SDL_Surface *destination) {

  // display some floors
  int src_y = (15 - highest_floor_displayed) * 12;
  int src_height = nb_floors_displayed * 12 + 1;

  int dst_y = 70 + (7 - nb_floors_displayed) * 6;

  SDL_Rect src_position = {96, src_y, 32, src_height};
  SDL_Rect dst_position = {79, dst_y};
  SDL_BlitSurface(dungeon_floors_img, &src_position, destination, &dst_position);

  // display the current floor with other colors
  src_position.x = 64;
  src_position.y = (15 - selected_floor) * 12;
  src_position.h = 13;

  dst_position.y = dst_y + (highest_floor_displayed - selected_floor) * 12;
  SDL_BlitSurface(dungeon_floors_img, &src_position, destination, &dst_position);

  // display the hero's icon
  int lowest_floor_displayed = highest_floor_displayed - nb_floors_displayed + 1;
  if (hero_floor >= lowest_floor_displayed && hero_floor <= highest_floor_displayed) {

    int y = dst_y + (highest_floor_displayed - hero_floor) * 12;
    hero_head_sprite->display(destination, 61, y);
  }

  // display the boss icon
  if (boss_floor >= lowest_floor_displayed && boss_floor <= highest_floor_displayed) {

    int boss_y = dst_y + (highest_floor_displayed - boss_floor) * 12 + 3;

    SDL_Rect src_position = {78, 0, 8, 8};
    SDL_Rect dst_position = {113, boss_y};

    SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  }

  // display the arrows
  if (lowest_floor_displayed > lowest_floor) {
    down_arrow_sprite->display(destination, 89, 151);
  }

  if (highest_floor_displayed < highest_floor) {
    up_arrow_sprite->display(destination, 89, 66);
  }
}

