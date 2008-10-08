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
#include "entities/Link.h"
#include "Counter.h"

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

  Map *map = game->get_current_map();
  link_position = *map->get_location();

  // outside map or inside map (no dungeon): show the world map
  if (dungeon == NULL) {

    set_caption_text("Carte du monde");

    const SDL_Rect *real_size = game->get_outside_world_size();

    if (map->is_in_outside_world()) {
      link_position.x += game->get_link()->get_x();
      link_position.y += game->get_link()->get_y();
    }

    link_position.x = link_position.x * outside_world_minimap_size.w / real_size->w;
    link_position.y = link_position.y * outside_world_minimap_size.h / real_size->h;

    link_position.x += 48 - 8;
    link_position.y += 59 - 6;

    if (equipment->has_world_map()) {
      world_map_img = ResourceManager::load_image("menus/outside_world_map.png");
      world_minimap_visible_y = MIN(388 - 133, MAX(0, link_position.y - 66));
    }
    else {
      world_map_img = ResourceManager::load_image("menus/outside_world_clouds.png");
      world_minimap_visible_y = 0;
    }
    moving_visible_y = 0;
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

    link_floor = game->get_current_map()->get_floor();
    boss_floor = dungeon->get_boss()->floor;

    highest_floor = dungeon->get_highest_floor();
    lowest_floor = dungeon->get_lowest_floor();
    nb_floors = dungeon->get_nb_floors();

    nb_floors_displayed = dungeon->get_nb_floors_displayed();
    highest_floor_displayed = dungeon->get_highest_floor_displayed(link_floor);
    selected_floor = link_floor;

    // map
    dungeon_map_img = NULL;
    if (dungeon_equipment->has_map()) {
      load_dungeon_map_image();
    }

    // Link's position
    if (dungeon_equipment->has_compass()) {

      link_point_sprite = new Sprite("menus/link_point");

      const SDL_Rect *floor_size = dungeon->get_floor_size(selected_floor);

      link_position.x += game->get_link()->get_x();
      link_position.y += game->get_link()->get_y();

      link_position.x = link_position.x * 123 / floor_size->w;
      link_position.y = link_position.y * 199 / floor_size->h;

      link_position.x += 48 - 1;
      link_position.y += 59 - 1;
    }
    else {
      link_point_sprite = NULL;
    }
  }

  link_head_sprite = new Sprite("menus/link_head");
  ostringstream oss;
  oss << "tunic" << equipment->get_tunic();
  link_head_sprite->set_current_animation(oss.str());

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

    if (dungeon_map_img != NULL) {
      SDL_FreeSurface(dungeon_map_img);
    }

    if (link_point_sprite != NULL) {
      delete link_point_sprite;
    }
  }

  delete link_head_sprite;
  delete up_arrow_sprite;
  delete down_arrow_sprite;
}

/**
 * Deletes any previous dungeon map image and loads the image
 * corresponding to the selected floor.
 */
void PauseSubmenuMap::load_dungeon_map_image(void) {

  if (dungeon_map_img != NULL) {
    SDL_FreeSurface(dungeon_map_img);
  }

  ostringstream oss;
  oss << "dungeons/map" << dungeon->get_number() << "_" << selected_floor << ".png";
  dungeon_map_img = ResourceManager::load_image(oss.str());
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param keysym the key pressed
 */
void PauseSubmenuMap::key_pressed(const SDL_keysym &keysym) {

  switch (keysym.sym) {

  case SDLK_LEFT:
    pause_menu->show_left_submenu();
    break;

  case SDLK_RIGHT:
    pause_menu->show_right_submenu();
    break;

  case SDLK_UP:
  case SDLK_DOWN:

    if (dungeon == NULL) {

      // move the world map
      if (equipment->has_world_map()) {
	moving_visible_y = (keysym.sym == SDLK_UP) ? -1 : 1;
	next_moving_visible_y_date = SDL_GetTicks();
      }
    }

    else {
      // select another floor
      int new_selected_floor = selected_floor + ((keysym.sym == SDLK_UP) ? 1 : -1);
      if (new_selected_floor >= lowest_floor && new_selected_floor <= highest_floor) {

	ResourceManager::get_sound("cursor")->play();
	link_head_sprite->restart_animation();
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

  link_head_sprite->update_current_frame();
  up_arrow_sprite->update_current_frame();
  down_arrow_sprite->update_current_frame();

  if (dungeon == NULL) {

    Uint8 *key_state = SDL_GetKeyState(NULL);
    if (moving_visible_y == -1
	&& (!key_state[SDLK_UP] || world_minimap_visible_y == 0)) {
      moving_visible_y = key_state[SDLK_DOWN] ? 1 : 0;
    }
    else if (moving_visible_y == 1
	     && (!key_state[SDLK_DOWN] || world_minimap_visible_y == 388 - 133)) {
      moving_visible_y = key_state[SDLK_UP] ? -1 : 0;
    }

    Uint32 now = SDL_GetTicks();
    if (moving_visible_y != 0 && now >= next_moving_visible_y_date) {
      world_minimap_visible_y += moving_visible_y;
      next_moving_visible_y_date += 10;
    }
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

    // display Link's position
    int link_visible_y = link_position.y - world_minimap_visible_y;
    if (link_visible_y >= 51 && link_visible_y <= 184) {
      link_head_sprite->display(destination, link_position.x, link_position.y - world_minimap_visible_y);
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
  if (dungeon_map_img != NULL) {
    SDL_Rect dst_position = {143, 66};
    SDL_BlitSurface(dungeon_map_img, NULL, destination, &dst_position);
  }

  if (dungeon_equipment->has_compass()) {

    // display Link's position
    if (link_floor == selected_floor) {
      link_point_sprite->display(destination, link_position.x, link_position.y);
    }
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
    SDL_Rect dst_position = {70, 168};
    SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  }

  // big key
  if (dungeon_equipment->has_big_key()) {
    SDL_Rect src_position = {34, 0, 17, 17};
    SDL_Rect dst_position = {90, 168};
    SDL_BlitSurface(dungeon_map_icons, &src_position, destination, &dst_position);
  }

  // boss key
  if (dungeon_equipment->has_boss_key()) {
    SDL_Rect src_position = {51, 0, 17, 17};
    SDL_Rect dst_position = {109, 168};
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

  // display Link's icon
  int lowest_floor_displayed = highest_floor_displayed - nb_floors_displayed + 1;
  if (link_floor >= lowest_floor_displayed && link_floor <= highest_floor_displayed) {

    int y = dst_y + (highest_floor_displayed - link_floor) * 12;
    link_head_sprite->display(destination, 61, y);
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

