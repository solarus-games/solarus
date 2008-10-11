#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Color.h"

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuQuestStatus::PauseSubmenuQuestStatus(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {

  // draw the items on a surface
  quest_items_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 225, 133, 32, 0, 0, 0, 0);
  SDL_SetColorKey(quest_items_surface, SDL_SRCCOLORKEY, Color::black);
  SDL_FillRect(quest_items_surface, NULL, Color::black);

  Equipment *equipment = game->get_equipment();
  SDL_Surface *treasures_img = ResourceManager::load_image("hud/message_and_treasure_icons.png");

  // tunic
  {
    int tunic = equipment->get_tunic();
    SDL_Rect src_position = {tunic * 16, 96, 16, 16};
    SDL_Rect dst_position = {0, 0}; // TODO
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);
  }

  // sword
  if (equipment->has_sword()) {
    int sword = equipment->get_sword();
    SDL_Rect src_position = {80 + sword * 16, 96, 16, 16};
    SDL_Rect dst_position = {0, 0}; // TODO
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);
  }

  // shield
  if (equipment->has_shield()) {
    int shield = equipment->get_shield();
    SDL_Rect src_position = {32 + shield * 16, 96, 16, 16};
    SDL_Rect dst_position = {0, 0}; // TODO
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);
  }

  // rupee bag
  {
    int max_rupees = equipment->get_max_rupees();
    int rupee_bag = (max_rupees == 100) ? 1 : ((max_rupees == 300) ? 2 : 3);
    
    SDL_Rect src_position = {rupee_bag * 16, 80, 16, 16};
    SDL_Rect dst_position = {0, 0}; // TODO
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);
  }

  // bomb bag
  int max_bombs = equipment->get_max_bombs();
  if (max_bombs != 0) {
    int bomb_bag = (max_bombs == 10) ? 1 : ((max_bombs == 30) ? 2 : 3);
    
    SDL_Rect src_position = {48 + bomb_bag * 16, 80, 16, 16};
    SDL_Rect dst_position = {0, 0}; // TODO
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);
  }

  // quiver
  int max_arrows = equipment->get_max_arrows();
  if (max_arrows != 0) {
    int quiver = (max_arrows == 10) ? 1 : ((max_arrows == 30) ? 2 : 3);
    
    SDL_Rect src_position = {96 + quiver * 16, 80, 16, 16};
    SDL_Rect dst_position = {0, 0}; // TODO
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);
  }

  // world map
  if (equipment->has_world_map()) {
    SDL_Rect src_position = {0, 80, 16, 16};
    SDL_Rect dst_position = {0, 0}; // TODO
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);
  }

  // pieces of heart
  // TODO

  // dungeons finished
  // TODO

  SDL_FreeSurface(treasures_img);
}

/**
 * Destructor.
 */
PauseSubmenuQuestStatus::~PauseSubmenuQuestStatus(void) {

  SDL_FreeSurface(quest_items_surface);
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuQuestStatus::key_pressed(Controls::GameKey key) {

  switch (key) {

  case Controls::LEFT:
    pause_menu->show_left_submenu();
    break;

  case Controls::RIGHT:
    pause_menu->show_right_submenu();
    break;

  default:
    break;
  }
}

/**
 * Updates this submenu.
 */
void PauseSubmenuQuestStatus::update(void) {
  
}

/**
 * Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuQuestStatus::display(SDL_Surface *destination) {
  PauseSubmenu::display(destination);

  // quest items
  SDL_BlitSurface(quest_items_surface, NULL, destination, NULL);
}
