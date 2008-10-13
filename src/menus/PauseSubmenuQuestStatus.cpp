#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseMenu.h"
#include "Game.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Color.h"
#include "Sound.h"
#include "Sprite.h"

// TODO: load this from some external file (for future translation)
static const string rupee_bag_texts[] = {"Sac de Rubis", "Grand\nSac de Rubis", "Sac de Rubis\nGéant"};
static const string bomb_bag_texts[] = {"Sac de Bombes", "Grand\nSac de Bombes", "Sac de Bombes\nGéant"};
static const string quiver_texts[] = {"Carquois", "Grand Carquois", "Carquois Géant"};
static const string world_map_text = "Carte du Monde";
static const string pieces_of_heart_text = "Fragements de Coeur";
static const string tunic_texts[] = {"Cotte de Mailles\nverte", "Cotte de Mailles\nbleue", "Cottes de Mailles\nrouge"};
static const string sword_texts[] = {"Epée du Courage", "Glaive Hylien", "Sabre du Héros", "Lame Purificatrice"};
static const string shield_texts[] = {"Bouclier", "Bouclier de Feu", "Bouclier Miroir"};

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuQuestStatus::PauseSubmenuQuestStatus(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {

  // cursor
  cursor_sprite = new Sprite("menus/pause_cursor");
  cursor_position = -1;
  set_cursor_position(0);

  // draw the items on a surface
  quest_items_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  SDL_SetColorKey(quest_items_surface, SDL_SRCCOLORKEY, Color::black);
  SDL_FillRect(quest_items_surface, NULL, Color::black);

  Equipment *equipment = game->get_equipment();
  SDL_Surface *treasures_img = ResourceManager::load_image("hud/message_and_treasure_icons.png");
  SDL_Surface *pieces_of_heart_img = ResourceManager::load_image("menus/quest_status_pieces_of_heart.png");

  // tunic
  {
    int tunic = equipment->get_tunic();
    SDL_Rect src_position = {tunic * 16, 96, 16, 16};
    SDL_Rect dst_position = {177, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[5] = tunic_texts[tunic];
  }

  // sword
  if (equipment->has_sword()) {
    int sword = equipment->get_sword();
    SDL_Rect src_position = {80 + sword * 16, 96, 16, 16};
    SDL_Rect dst_position = {211, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[6] = sword_texts[sword - 1];
  }

  // shield
  if (equipment->has_shield()) {
    int shield = equipment->get_shield();
    SDL_Rect src_position = {32 + shield * 16, 96, 16, 16};
    SDL_Rect dst_position = {245, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[7] = shield_texts[shield - 1];
  }

  // rupee bag
  {
    int max_rupees = equipment->get_max_rupees();
    int rupee_bag = (max_rupees == 100) ? 1 : ((max_rupees == 300) ? 2 : 3);
    
    SDL_Rect src_position = {rupee_bag * 16, 80, 16, 16};
    SDL_Rect dst_position = {60, 71};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[0] = rupee_bag_texts[rupee_bag - 1];
  }

  // bomb bag
  int max_bombs = equipment->get_max_bombs();
  if (max_bombs != 0) {
    int bomb_bag = (max_bombs == 10) ? 1 : ((max_bombs == 30) ? 2 : 3);
    SDL_Rect src_position = {48 + bomb_bag * 16, 80, 16, 16};
    SDL_Rect dst_position = {60, 100};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[1] = bomb_bag_texts[bomb_bag - 1];
  }

  // quiver
  int max_arrows = equipment->get_max_arrows();
  if (max_arrows != 0) {
    int quiver = (max_arrows == 10) ? 1 : ((max_arrows == 30) ? 2 : 3);
    
    SDL_Rect src_position = {96 + quiver * 16, 80, 16, 16};
    SDL_Rect dst_position = {60, 130};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[2] = quiver_texts[quiver - 1];
  }

  // world map
  if (equipment->has_world_map()) {
    SDL_Rect src_position = {0, 80, 16, 16};
    SDL_Rect dst_position = {60, 164};
    SDL_BlitSurface(treasures_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[3] = world_map_text;
  }

  // pieces of heart
  {
    int x = 51 * equipment->get_nb_pieces_of_heart();
    SDL_Rect src_position = {x, 0, 51, 50};
    SDL_Rect dst_position = {101, 82};
    SDL_BlitSurface(pieces_of_heart_img, &src_position, quest_items_surface, &dst_position);

    caption_texts[4] = pieces_of_heart_text;
  }

  // dungeons finished
  // TODO

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
