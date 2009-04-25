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
#include "Treasure.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DungeonEquipment.h"
#include "DialogBox.h"
#include "Counter.h"
#include "Map.h"
#include "MapScript.h"

/**
 * Creates a new treasure without amount.
 * @param content content of the treasure
 * @param savegame_variable index of the savegame boolean indicating that the hero has found this treasure
 * or -1 if this treasure is not saved
 */
Treasure::Treasure(Content content, int savegame_variable):
  content(content), amount(1), savegame_variable(savegame_variable), counter(NULL) {

  treasures_img = ResourceManager::load_image("hud/message_and_treasure_icons.png");
}

/**
 * Creates a new treasure.
 * @param content content of the treasure
 * @param amount for bombs, arrows, apples, pains au chocolat, croissants, hearts, 
 * green rupees, blue rupees and red rupees: indicates the amount;
 * if the amount is greater than 1, a counter will be shown.
 * @param savegame_variable index of the savegame boolean indicating that the hero has found this treasure
 * or -1 if this treasure is not saved
 */
Treasure::Treasure(Content content, int amount, int savegame_variable):
  content(content), amount(amount), savegame_variable(savegame_variable), counter(NULL) {

  treasures_img = ResourceManager::load_image("hud/message_and_treasure_icons.png");

  if (has_amount() && amount > 1) {
    counter = new Counter(3, false, 0, 0);
    counter->set_value(amount);
  }
}

/**
 * Destructor.
 */
Treasure::~Treasure(void) {

  SDL_FreeSurface(treasures_img);

  if (counter != NULL) {
    delete counter;
  }
}

/**
 * Returns the content of this treasure.
 * @return this treasure's content
 */
Treasure::Content Treasure::get_content(void) {
  return content;
}

/**
 * Returns the index of the variable where this treasure is saved.
 * @return the savegame variable of this treasure
 */
int Treasure::get_savegame_variable(void) {
  return savegame_variable;
}

/**
 * Returns whether this treasure content has an amount value.
 * This only depends on the kind of content.
 * @return true if there is an amount with this kind of content
 */
bool Treasure::has_amount(void) {

  bool result;

  switch (content) {

  case BOMBS:
  case ARROWS:
  case APPLES:
  case PAINS_AU_CHOCOLAT:
  case CROISSANTS:
  case HEARTS:
  case GREEN_RUPEES:
  case BLUE_RUPEES:
  case RED_RUPEES:
    result = true;
    break;

  default:
    result = false;
  }

  return result;
}

/**
 * Returns the amount of this treasure.
 * @return the amount
 */
int Treasure::get_amount(void) {
  return amount;
}

/**
 * Returns whether the player already has the maximum amount
 * of this item.
 * @return true if the player already has the maximum amount
 * of this item
 */
bool Treasure::is_amount_full(void) {

  bool full = false;
  Equipment *equipment = zsdx->game->get_equipment();

  switch (content) {

  case APPLES:
    full = equipment->has_inventory_item_maximum_amount(INVENTORY_APPLES);
    break;

  case PAINS_AU_CHOCOLAT:
    full = equipment->has_inventory_item_maximum_amount(INVENTORY_PAINS_AU_CHOCOLAT);
    break;

  case CROISSANTS:
    full = equipment->has_inventory_item_maximum_amount(INVENTORY_CROISSANTS);
    break;

  case BOMBS:
    full = equipment->has_inventory_item_maximum_amount(INVENTORY_BOMBS);
    break;

  case ARROWS:
    full = equipment->has_inventory_item_maximum_amount(INVENTORY_BOW);
    break;

  default:
    break;
  }

  return full;
}

/**
 * Returns whether the player has got this treasure according to the savegame.
 * Returns false if the treasure possession state is not saved.
 * @return true if the player has found this treasure
 */
bool Treasure::is_found(void) {
  return savegame_variable != -1 && zsdx->game->get_savegame()->get_boolean(savegame_variable);
}

/**
 * Give the treasure to the player: plays the treasure sound, makes the hero
 * brandish the item and adds the item to the hero's equipment.
 * The treasure content should not be NONE.
 */
void Treasure::give_to_player(void) {

  if (content != NONE) {
    play_treasure_sound();
    show_message();
    add_item_to_equipment();
    zsdx->game->get_current_script()->event_got_treasure(content, savegame_variable);
  }
}

/**
 * Plays a treasure sound.
 */
void Treasure::play_treasure_sound(void) {
  
  // the treasure sound is the same for all items except the piece of heart and the heart container
  std::string sound_name;

  if (content != PIECE_OF_HEART && content != HEART_CONTAINER) {
    sound_name = "treasure";
  }
  else {
    sound_name = "piece_of_heart";
  }
  ResourceManager::get_sound(sound_name)->play();
}

/**
 * Shows the message telling that the player
 * has found the item.
 */
void Treasure::show_message(void) {

  Game *game = zsdx->game;
  Equipment *equipment = game->get_equipment();

  // the message id is _treasure_x where x is the treasure content
  std::ostringstream oss;
  oss << "_treasure_" << content;

  // but for some kinds of content, the message id is more complex and has a suffix
  if (content == PIECE_OF_HEART) {

    // for a piece of heart, the message shown depends on their number
    int nb_pieces = equipment->get_nb_pieces_of_heart() + 1;
    oss << "_" << nb_pieces;
  }

  else if (has_amount()) {

    /* for an item with an amount (e.g. 10 bombs), if the amount
     * is 1 we must display a message with the singular form, and
     * if the amount is greater than 1 we must use the plural form.
     */

    if (amount == 1) {
      oss << "_1";
    }
    else {
      oss << "_2";
    }
  }

  std::string message_id = oss.str();
  game->show_message(message_id);

  if (has_amount() && amount > 1) {
    game->get_dialog_box()->set_variable(message_id, amount);
  }
}

/**
 * Adds the item to the hero's eqipment.
 */
void Treasure::add_item_to_equipment(void) {

  Game *game = zsdx->game;
  Savegame *savegame = game->get_savegame();
  Equipment *equipment = game->get_equipment();
  DungeonEquipment *dungeon_equipment = game->get_dungeon_equipment();

  // mark the treasure as found in the savegame
  if (savegame_variable != -1) {
    savegame->set_boolean(savegame_variable, true);
  }

  // give the item
  switch (content) {

  case NONE:
    DIE("Cannot give an empty treasure");
    break;
    
  case FEATHER:
    equipment->give_inventory_item(INVENTORY_FEATHER);
    break;

  case BOW:
    equipment->give_inventory_item(INVENTORY_BOW);
    equipment->set_max_arrows(10);
    break;

  case BOW_AND_ARROWS:
    equipment->give_inventory_item(INVENTORY_BOW);
    equipment->set_max_arrows(10);
    equipment->add_arrows(10);
    break;

  case BOOMERANG:
    equipment->give_inventory_item(INVENTORY_BOOMERANG);
    break;

  case LAMP:
    equipment->give_inventory_item(INVENTORY_LAMP);
    break;

  case HOOK_SHOT:
    equipment->give_inventory_item(INVENTORY_HOOK_SHOT);
    break;

  case BOTTLE:
    equipment->add_bottle();
    break;

  case WATER:
    equipment->give_inventory_item(equipment->get_first_empty_bottle(), 2);
    break;

  case RED_POTION:
    equipment->give_inventory_item(equipment->get_first_empty_bottle(), 3);
    break;


  case GREEN_POTION:
    equipment->give_inventory_item(equipment->get_first_empty_bottle(), 4);
    break;

  case BLUE_POTION:
    equipment->give_inventory_item(equipment->get_first_empty_bottle(), 5);
    break;

  case FAIRY_IN_BOTTLE:
    equipment->give_inventory_item(equipment->get_first_empty_bottle(), 6);
    break;

  case PEGASUS_SHOES:
    equipment->give_inventory_item(INVENTORY_PEGASUS_SHOES);
    break;

  case MYSTIC_MIRROR:
    equipment->give_inventory_item(INVENTORY_MYSTIC_MIRROR);
    break;

  case CANE_OF_SOMARIA:
    equipment->give_inventory_item(INVENTORY_CANE_OF_SOMARIA);
    break;

  case MAGIC_CAPE:
    equipment->give_inventory_item(INVENTORY_MAGIC_CAPE);
    break;

  case IRON_GLOVE:
    equipment->give_inventory_item(INVENTORY_GLOVE, 1);
    break;

  case GOLDEN_GLOVE:
    equipment->give_inventory_item(INVENTORY_GLOVE, 2);
    break;

  case FIRE_STONE:
    equipment->add_inventory_item_amount(INVENTORY_FIRE_STONES, 1);
    break;


  case APPLES:
    equipment->give_inventory_item(INVENTORY_APPLES);
    equipment->add_inventory_item_amount(INVENTORY_APPLES, amount);
    break;

  case PAINS_AU_CHOCOLAT:
    equipment->give_inventory_item(INVENTORY_PAINS_AU_CHOCOLAT);
    equipment->add_inventory_item_amount(INVENTORY_PAINS_AU_CHOCOLAT, amount);
    break;

  case CROISSANTS:
    equipment->give_inventory_item(INVENTORY_CROISSANTS);
    equipment->add_inventory_item_amount(INVENTORY_CROISSANTS, amount);
    break;

  case APPLE_PIE:
    equipment->give_inventory_item(INVENTORY_L4_WAY_BONE_KEY, 1);
    break;

  case GOLDEN_BARS:
    equipment->give_inventory_item(INVENTORY_L4_WAY_BONE_KEY, 2);
    break;

  case EDELWEISS:
    equipment->give_inventory_item(INVENTORY_L4_WAY_BONE_KEY, 3);
    break;

  case BONE_KEY:
    equipment->give_inventory_item(INVENTORY_L4_WAY_BONE_KEY, 4);
    break;

  case FLIPPERS:
    equipment->give_inventory_item(INVENTORY_FLIPPERS);
    break;

  case RED_KEY:
    equipment->give_inventory_item(INVENTORY_RED_KEY);
    break;


  case CLAY_KEY:
    equipment->give_inventory_item(INVENTORY_CLAY_KEY);
    break;

  case ROCK_KEY:
    equipment->give_inventory_item(INVENTORY_ROCK_KEY);
    break;

  case IRON_KEY:
    equipment->give_inventory_item(INVENTORY_IRON_KEY);
    break;

  case STONE_KEY:
    equipment->give_inventory_item(INVENTORY_STONE_KEY);
    break;

  case WOODEN_KEY:
    equipment->give_inventory_item(INVENTORY_WOODEN_KEY);
    break;

  case ICE_KEY:
    equipment->give_inventory_item(INVENTORY_ICE_KEY);
    break;


  case WORLD_MAP:
    equipment->add_world_map();
    break;

  case LARGE_RUPEE_BAG:
    equipment->set_max_rupees(199);
    break;

  case HUGE_RUPEE_BAG:
    equipment->set_max_rupees(999);
    break;

  case SMALL_BOMB_BAG:
    equipment->set_max_bombs(10);
    equipment->give_inventory_item(INVENTORY_BOMBS);
    equipment->add_bombs(10);
    break;

  case LARGE_BOMB_BAG:
    equipment->set_max_bombs(30);
    break;

  case HUGE_BOMB_BAG:
    equipment->set_max_bombs(99);
    break;

  case LARGE_QUIVER:
    equipment->set_max_arrows(30);
    break;

  case HUGE_QUIVER:
    equipment->set_max_arrows(99);
    break;


  case BLUE_TUNIC:
    equipment->set_tunic(1);
    break;

  case RED_TUNIC:
    equipment->set_tunic(2);
    break;

  case SHIELD_1:
    equipment->set_shield(1);
    break;

  case SHIELD_2:
    equipment->set_shield(2);
    break;

  case SHIELD_3:
    equipment->set_shield(3);
    break;

  case SWORD_1:
    equipment->set_sword(1);
    break;

  case SWORD_2:
    equipment->set_sword(2);
    break;

  case SWORD_3:
    equipment->set_sword(3);
    break;

  case SWORD_4:
    equipment->set_sword(4);
    break;


  case MAP:
    dungeon_equipment->add_map();
    break;

  case COMPASS:
    dungeon_equipment->add_compass();
    break;

  case SMALL_KEY:
    equipment->add_small_key();
    break;

  case BIG_KEY:
    dungeon_equipment->add_big_key();
    break;

  case BOSS_KEY:
    dungeon_equipment->add_boss_key();
    break;

  case PIECE_OF_HEART:
    equipment->add_piece_of_heart();
    break;


  case HEART_CONTAINER:
    equipment->add_heart_container();
    break;

  case BOMBS:
    equipment->add_inventory_item_amount(INVENTORY_BOMBS, amount);
    break;

  case ARROWS:
    equipment->add_inventory_item_amount(INVENTORY_BOW, amount);
    break;

  case HEARTS:
    equipment->add_hearts(amount * 4);
    break;

  case SMALL_MAGIC:
    equipment->add_magic(6);
    break;

  case BIG_MAGIC:
    equipment->add_magic(42);
    break;

  case GREEN_RUPEES:
    equipment->add_rupees(amount);
    break;

  case BLUE_RUPEES:
    equipment->add_rupees(5 * amount);
    break;

  case RED_RUPEES:
    equipment->add_rupees(20 * amount);
    break;
  }
}

/**
 * Displays the treasure.
 * @param destination the surface where to draw
 * @param x the treasure x position on this surface
 * @param y the treasure y position on this surface
 */
void Treasure::display(SDL_Surface *destination, int x, int y) {

  // display the item
  SDL_Rect src_position = {0, 0, 16, 16};
  src_position.x = 16 * (content % 10);
  src_position.y = 16 * (content / 10);

  SDL_Rect dst_position = {x, y, 0, 0};

  SDL_BlitSurface(treasures_img, &src_position, destination, &dst_position);

  // display the counter
  if (counter != NULL) {
    counter->display(destination, x - 4, y + 12);
  }
}

