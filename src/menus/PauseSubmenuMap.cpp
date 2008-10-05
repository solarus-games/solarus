#include "menus/PauseSubmenuMap.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Dungeon.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "Map.h"
#include "Equipment.h"
#include "entities/Link.h"

const SDL_Rect PauseSubmenuMap::outside_world_minimap_size = {0, 0, 225, 388};

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuMap::PauseSubmenuMap(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game), dungeon(game->get_current_dungeon()) {

  Map *map = game->get_current_map();
  link_position = *map->get_location();

  if (dungeon == NULL) {
    world_map_img = ResourceManager::load_image("menus/outside_world_map.png");
    set_caption_text("Carte du monde");

    const SDL_Rect *real_size = game->get_outside_world_size();

    if (map->is_in_outside_world()) {
      link_position.x += game->get_link()->get_x();
      link_position.y += game->get_link()->get_y();
    }

    link_position.x = link_position.x * outside_world_minimap_size.w / real_size->w;
    link_position.y = link_position.y * outside_world_minimap_size.h / real_size->h;
  }
  else {
    // TODO
  }

  link_position.x += 48 - 8;
  link_position.y += 59 - 7;

  link_head_sprite = new Sprite("menus/link_head");
  ostringstream oss;
  oss << "tunic" << equipment->get_tunic();
  link_head_sprite->set_current_animation(oss.str());
}

/**
 * Destructor.
 */
PauseSubmenuMap::~PauseSubmenuMap(void) {

  if (dungeon == NULL) {
    SDL_FreeSurface(world_map_img);
  }

  delete link_head_sprite;
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

  default:
    break;
  }
}

/**
 * Updates this submenu.
 */
void PauseSubmenuMap::update(void) {
  link_head_sprite->update_current_frame();
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

  // display the map
  SDL_Rect src_position = {0, 0, 225, 133};
  static SDL_Rect dst_position = {48, 59, 0, 0};

  SDL_BlitSurface(world_map_img, &src_position, destination, &dst_position);

  // display Link's position
  link_head_sprite->display(destination, link_position.x, link_position.y);
}

/**
 * Displays the dungeon map.
 * @param destination the destination surface
 */
void PauseSubmenuMap::display_dungeon_map(SDL_Surface *destination) {

}
