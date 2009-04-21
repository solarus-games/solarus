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
#include "Equipment.h"
#include "ZSDX.h"
#include "Game.h"
#include "Savegame.h"
#include "DialogBox.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Map.h"
#include "Treasure.h"
#include "InventoryItem.h"

/**
 * Constructor.
 */
Equipment::Equipment(Savegame *savegame):
  savegame(savegame), magic_decrease_delay(0), 
  giving_fairy(false), giving_water(false) {
}

/**
 * Destructor.
 */
Equipment::~Equipment(void) {

}

/**
 * This function is be called repeatedly by the game.
 * Most of the time, there is nothing to update in this
 * class. The only element updated here is the magic bar
 * when it decreases continuously.
 */
void Equipment::update(void) {

  // magic bar
  if (magic_decrease_delay != 0) {
    // the magic bar is decreasing

    if (!zsdx->game->is_suspended()) {

      if (SDL_GetTicks() > next_magic_decrease_date) {

	remove_magic(1);

	if (get_magic() > 0) {
	  next_magic_decrease_date += magic_decrease_delay;
	}
	else {
	  stop_removing_magic();
	}
      }
    }
    else {
      // delay the next decrease while the game is suspended
      next_magic_decrease_date = SDL_GetTicks();
    }
  }

  // fairy
  if (giving_fairy) {

    if (!zsdx->game->is_showing_message()) {

      giving_fairy = false;
      int answer = zsdx->game->get_dialog_box()->get_last_answer();

      if (answer == 0) {
	// restore the hearts
	add_hearts(7);
      }
      else {
	// keep the fairy in a bottle
	if (!has_empty_bottle()) {
	  zsdx->game->show_message("_found_fairy.no_empty_bottle");
	  ResourceManager::get_sound("wrong")->play();
	}
	else {
	  give_inventory_item(get_first_empty_bottle(), 6);
	  ResourceManager::get_sound("danger")->play();
	}
      }
    }
  }
  else if (giving_water && !zsdx->game->is_showing_message()) {

    giving_water = false;
    int answer = zsdx->game->get_dialog_box()->get_last_answer();

    if (answer == 0) {
      zsdx->game->give_treasure(new Treasure(Treasure::WATER, -1));
    }
  }
}

// tunic

/**
 * Returns the number of the player's tunic.
 * @return the player's tunic number (0: green tunic,
 * 1: blue tunic, 2: red tunic)
 */
int Equipment::get_tunic(void) {
  return savegame->get_integer(Savegame::HERO_TUNIC);
}

/**
 * Sets the player's tunic.
 * @param tunic the player's tunic number (0: green tunic,
 * 1: blue tunic, 2: red tunic)
 */
void Equipment::set_tunic(int tunic) {

  if (tunic != get_tunic()) {
    // the tunic has changed

    if (tunic < 0 || tunic > 2) {
      DIE("Illegal tunic number: " << tunic);
    }

    savegame->set_integer(Savegame::HERO_TUNIC, tunic);
  }
}

// sword

/**
 * Returns whether the player has a sword.
 * @return true if the player has a sword, i.e. if get_sword_number() > 0
 */
bool Equipment::has_sword(void) {
  return get_sword() > 0;
}

/**
 * Returns the number of the player's sword.
 * @return the player's sword number (0: no sword,
 * 1 to 4: sword 1 to 4)
 */
int Equipment::get_sword(void) {
  return savegame->get_integer(Savegame::HERO_SWORD);
}

/**
 * Sets the player's sword.
 * @param sword the player's sword number (0: no sword,
 * 1 to 4: sword 1 to 4)
 */
void Equipment::set_sword(int sword) {

  if (sword != get_sword()) {
    // the sword has changed

    if (sword < 0 || sword > 4) {
      DIE("Illegal sword number: " << sword);
    }

    savegame->set_integer(Savegame::HERO_SWORD, sword);
  }
}

// shield

/**
 * Returns whether the player has a shield.
 * @return true if the player has a shield, i.e. if get_shield_number() > 0
 */
bool Equipment::has_shield(void) {
  return get_shield() > 0;
}

/**
 * Returns the number of the player's shield.
 * @return the player's shield number (0: no shield,
 * 1 to 3: shield 1 to 3)
 */
int Equipment::get_shield(void) {
  return savegame->get_integer(Savegame::HERO_SHIELD);
}

/**
 * Sets the player's shield.
 * @param shield the player's shield number (0: no shield,
 * 1 to 3: shield 1 to 3)
 */
void Equipment::set_shield(int shield) {

  if (shield != get_shield()) {
    // the shield has changed

    if (shield < 0 || shield > 3) {
      DIE("Illegal shield number: " << shield);
    }

    savegame->set_integer(Savegame::HERO_SHIELD, shield);
  }
}

// rupees

/**
 * Returns the maximum number of rupees of the player.
 * @return the player's maximum number of rupees (100, 300 or 999)
 */
int Equipment::get_max_rupees(void) {
  return savegame->get_integer(Savegame::MAX_RUPEES);
}

/**
 * Sets the maximum number of rupees of the player.
 * @param max_rupees the player's maximum number of rupees (100, 300 or 999)
 */
void Equipment::set_max_rupees(int max_rupees) {

  if (max_rupees != 100 && max_rupees != 300 && max_rupees != 999) {
    DIE("Illegal maximum number of rupees: " << max_rupees);
  }

  savegame->set_integer(Savegame::MAX_RUPEES, max_rupees);
}

/**
 * Returns the player's current number of rupees.
 * @return the player's current number of rupees
 */
int Equipment::get_rupees(void) {
  return savegame->get_integer(Savegame::CURRENT_RUPEES);
}

/**
 * Sets the player's current number of rupees.
 * Exits with an error message if the given number of rupees is not valid.
 * @param rupees the player's new number of rupees
 */
void Equipment::set_rupees(int rupees) {

  if (rupees < 0 || rupees > get_max_rupees()) {
    DIE("Illegal number of rupees: " << rupees);
  }

  savegame->set_integer(Savegame::CURRENT_RUPEES, rupees);
}

/**
 * Adds some rupees to the player.
 * If the maximum number of rupees is achieved, no more rupees are added.
 * @param rupees_to_add number of rupees to add
 */
void Equipment::add_rupees(int rupees_to_add) {

  int max_rupees = get_max_rupees();
  int total = get_rupees() + rupees_to_add;

  set_rupees(MIN(total, max_rupees));
}

/**
 * Removes some rupees from the player.
 * If the number of rupees achieves zero, no more rupees are removed.
 * @param rupees_to_remove number of rupees to remove
 */
void Equipment::remove_rupees(int rupees_to_remove) {

  int total = get_rupees() - rupees_to_remove;

  set_rupees(MAX(total, 0));
}

// hearts

/**
 * Returns the maximum number of hearts of the player.
 * @return the player's maximum number of rupees
 */
int Equipment::get_max_hearts(void) {
  return savegame->get_integer(Savegame::MAX_HEARTS);
}

/**
 * Sets the maximum number of hearts of the player.
 * The program exits with an error message if the given maximum
 * number of hearts is not valid.
 * @param max_hearts the player's maximum number of hearts
 */
void Equipment::set_max_hearts(int max_hearts) {

  if (max_hearts <= 0 || max_hearts > 20) {
    DIE("Illegal maximum number of hearts: " << max_hearts);
  }

  savegame->set_integer(Savegame::MAX_HEARTS, max_hearts);
}

/**
 * Adds a heart container to the player.
 */
void Equipment::add_heart_container(void) {
  set_max_hearts(get_max_hearts() + 1);
  restore_all_hearts();
}

/**
 * Returns the current number of hearts of the player.
 * The value returned is actually the number of heart quarters,
 * so a value of 4 means that the player has 1 heart left.
 * @return the player's current number of hearts (in heart quarters)
 */
int Equipment::get_hearts(void) {
  return savegame->get_integer(Savegame::CURRENT_HEARTS);
}

/**
 * Sets the current number of hearts of the player.
 * The given value is actually the number of heart quarters,
 * so a value of 4 means that the player has 1 heart left.
 * The program exits with an error message if the given number
 * of hearts is not valid.
 * @param hearts the player's new number of hearts (in heart quarters)
 */
void Equipment::set_hearts(int hearts) {

  if (hearts < 0 || hearts > get_max_hearts() * 4) {
    DIE("Illegal number of hearts: " << hearts);
  }

  savegame->set_integer(Savegame::CURRENT_HEARTS, hearts);
}

/**
 * Gives some hearts to the player.
 * If the maximum number of hearts is achieved, no more hearts are added.
 * @param hearts_to_add number of hearts to add (in heart quarters)
 */
void Equipment::add_hearts(int hearts_to_add) {

  int max_hearts = get_max_hearts() * 4;
  int total = get_hearts() + hearts_to_add;

  set_hearts(MIN(total, max_hearts));
}

/**
 * Removes some hearts from the player.
 * If the number of hearts achieves zero, the game over sequence is started.
 * @param hearts_to_remove number of hearts to remove
 */
void Equipment::remove_hearts(int hearts_to_remove) {

  int total = get_hearts() - hearts_to_remove;

  set_hearts(MAX(total, 0));
}

/**
 * Restores all the hearts.
 */
void Equipment::restore_all_hearts(void) {
  set_hearts(get_max_hearts() * 4);
}

/**
 * Returns whether the player is running out of hearts.
 * The function returns true if the number of hearts is
 * greater than zero and lower
 * than or equal to 25% of the maximum.
 * @return true if the player is running out of hearts
 */
bool Equipment::needs_hearts(void) {
  return get_hearts() > 0 && get_hearts() <= get_max_hearts();
}

/**
 * Returns the current number of pieces of heart of the player.
 * @returns the player's current number of pieces of heart, between 0 and 3
 */
int Equipment::get_nb_pieces_of_heart(void) {
  return savegame->get_integer(Savegame::PIECES_OF_HEART);
}

/**
 * Adds a piece of heart to the player.
 */
void Equipment::add_piece_of_heart(void) {

  // note: the piece_of_heart sound and the message are handled
  // by the Treasure class

  // check whether the player has a new heart

  int nb_pieces_of_heart = get_nb_pieces_of_heart() + 1;
  if (nb_pieces_of_heart < 4) {
    // no new heart
    savegame->set_integer(Savegame::PIECES_OF_HEART, nb_pieces_of_heart);
  }
  else {
    // new heart container
    savegame->set_integer(Savegame::PIECES_OF_HEART, 0);
    add_heart_container();
  }

  restore_all_hearts();
}

/**
 * Gives a fairy to the player: shows the dialog box and does the action
 * chosen by the player: restore his hearts or keep the fairy in a bottle.
 */
void Equipment::found_fairy(void) {

  if (has_bottle()) {
    zsdx->game->show_message("_found_fairy");
    giving_fairy = true;
    // the next messages will be handled by the update() function
  }
  else {
    add_hearts(7);
  }
}

/**
 * Gives some water to the player: shows the dialog box and give the water
 * to the player if he wants and if this is possible.
 */
void Equipment::found_water(void) {

  if (has_bottle()) {
    if (has_empty_bottle()) {
      zsdx->game->show_message("_found_water");
      giving_water = true;
      // the next messages will be handled by the update() function
    }
    else {
      zsdx->game->show_message("_found_water.no_empty_bottle");
    }
  }
  else {
    zsdx->game->show_message("_found_water.no_bottle");
  }
}

// magic

/**
 * Returns the maximum level of the player's magic bar.
 * @return the maximum level of magic (0, 42 or 84 points)
 */
int Equipment::get_max_magic(void) {
  return savegame->get_integer(Savegame::MAX_MAGIC);
}

/**
 * Sets the maximum level of the player's magic bar.
 * Exits with an error message if the value specified
 * if not valid.
 * @param max_magic the maximum level of magic (0, 42 or 84 points)
 */
void Equipment::set_max_magic(int max_magic) {

  if (max_magic != 0 && max_magic != 42 && max_magic != 84) {
    DIE("Illegal maximum number of magic points: " << max_magic);
  }

  savegame->set_integer(Savegame::MAX_MAGIC, max_magic);

  restore_all_magic();
}

/**
 * Returns the current number of magic points of the player.
 * @return the player's current number of magic points (0 to 28)
 */
int Equipment::get_magic(void) {
  return savegame->get_integer(Savegame::CURRENT_MAGIC);
}

/**
 * Sets the current number of magic points of the player.
 * The program exits with an error message if the given value
 * is not valid.
 * @param magic the player's new number of magic points (0 to 84)
 */
void Equipment::set_magic(int magic) {

  if (magic < 0 || magic > get_max_magic()) {
    DIE("Illegal number of magic points: " << magic);
  }

  savegame->set_integer(Savegame::CURRENT_MAGIC, magic);
}

/**
 * Adds some magic points to the player.
 * If the maximum value is achieved, no more magic points are added.
 * @param magic_to_add number of magic points to add
 */
void Equipment::add_magic(int magic_to_add) {

  int max_magic = get_max_magic();
  int total = get_magic() + magic_to_add;

  set_magic(MIN(total, max_magic));
}

/**
 * Removes some magic points from the player.
 * If the number of magic points achieves zero, no more magic points
 * are removed.
 * @param magic_to_remove number of magic poits to remove
 */
void Equipment::remove_magic(int magic_to_remove) {

  int total = get_magic() - magic_to_remove;

  set_magic(MAX(total, 0));
}

/**
 * Restores all magic points.
 */
void Equipment::restore_all_magic(void) {
  set_magic(get_max_magic());
}

/**
 * Returns whether the player needs magic.
 * The function returns true if the player has a magic bar
 * and has 0 magic points left.
 * @return true if the player needs magic
 */
bool Equipment::needs_magic(void) {
  return get_magic() == 0 && get_max_magic() != 0;
}

/**
 * Returns whether the magic bar is decreasing continuously.
 * @return true if the magic bar is decreasing, false otherwise
 */
bool Equipment::is_magic_decreasing(void) {
  return this->magic_decrease_delay != 0;
}

/**
 * Starts removing magic continuously.
 * @param delay delay in miliseconds between two decreases
 */
void Equipment::start_removing_magic(Uint32 delay) {

  if (delay <= 0) {
    DIE("Illegal magic bar decrease delay: " << delay);
  }

  if (get_magic() > 0) {
    this->magic_decrease_delay = delay;
    this->next_magic_decrease_date = SDL_GetTicks();

    // the magic points will be removed by the update() function
  }
}

/**
 * Stops removing magic continuously.
 */
void Equipment::stop_removing_magic(void) {
  this->magic_decrease_delay = 0;
}

// bombs

/**
 * Returns the maximum number of bombs of the player.
 * @return the player's maximum number of bombs (0, 10, 30 or 99)
 */
int Equipment::get_max_bombs(void) {
  return savegame->get_integer(Savegame::MAX_BOMBS);
}

/**
 * Sets the maximum number of bombs of the player.
 * @param max_bombs the player's maximum number of bombs (0, 10, 30 or 99)
 */
void Equipment::set_max_bombs(int max_bombs) {

  if (max_bombs != 0 && max_bombs != 10 && max_bombs != 30 && max_bombs != 99) {
    DIE("Illegal maximum number of bombs: " << max_bombs);
  }

  savegame->set_integer(Savegame::MAX_BOMBS, max_bombs);
}

/**
 * Returns the current number of bombs.
 * @return the player's current number of bombs
 */
int Equipment::get_bombs(void) {
  return savegame->get_integer(Savegame::CURRENT_BOMBS);
}

/**
 * Sets the current number of bombs of the player.
 * The program exits with an error message if the given number
 * of bombs is not valid.
 * @param bombs the player's new number of bombs
 */
void Equipment::set_bombs(int bombs) {

  if (bombs < 0 || bombs > get_max_bombs()) {
    DIE("Illegal number of bombs: " << bombs);
  }

  savegame->set_integer(Savegame::CURRENT_BOMBS, bombs);
}

/**
 * Adds some bombs to the player.
 * If the maximum number of bombs is achieved, no more bombs are added.
 * @param bombs_to_add number of bombs to add
 */
void Equipment::add_bombs(int bombs_to_add) {

  int max_bombs = get_max_bombs();
  int total = get_bombs() + bombs_to_add;

  set_bombs(MIN(total, max_bombs));
}

/**
 * Removes a bomb from the player.
 * If the number of bombs is already zero, nothing happens.
 */
void Equipment::remove_bomb(void) {

  int total = get_bombs() - 1;

  set_bombs(MAX(total, 0));
}

/**
 * Returns whether the player needs bombs.
 * The function returns true if the player has obtained the bombs
 * but has 0 bombs left.
 * @return true if the player needs bombs
 */
bool Equipment::needs_bombs(void) {
  return has_inventory_item(ITEM_BOMBS)
    && get_bombs() == 0;
}

/**
 * Returns the maximum number of arrows of the player.
 * @return the player's maximum number of arrows (0, 10, 30 or 99)
 */
int Equipment::get_max_arrows(void) {
  return savegame->get_integer(Savegame::MAX_ARROWS);
}

/**
 * Sets the maximum number of arrows of the player.
 * @param max_arrows the player's maximum number of arrows (0, 10, 30 or 99)
 */
void Equipment::set_max_arrows(int max_arrows) {

  if (max_arrows != 0 && max_arrows != 10 && max_arrows != 30 && max_arrows != 99) {
    DIE("Illegal maximum number of arrows: " << max_arrows);
  }

  savegame->set_integer(Savegame::MAX_ARROWS, max_arrows);
}

/**
 * Returns the current number of arrows.
 * @return the player's current number of arrows
 */
int Equipment::get_arrows(void) {
  return savegame->get_integer(Savegame::CURRENT_ARROWS);
}

/**
 * Sets the current number of arrows of the player.
 * The program exits with an error message if the given number
 * of arrows is not valid.
 * @param arrows the player's new number of arrows
 */
void Equipment::set_arrows(int arrows) {

  if (arrows < 0 || arrows > get_max_arrows()) {
    DIE("Illegal number of arrows: " << arrows);
  }

  savegame->set_integer(Savegame::CURRENT_ARROWS, arrows);

  if (has_inventory_item(ITEM_BOW)) {

    // the bow item changes depending on whether the player has arrows
    if (arrows == 0) {
      give_inventory_item(ITEM_BOW, 1); // bow without arrows
    }
    else {
      give_inventory_item(ITEM_BOW, 2); // bow with arrows
    }
  }
}

/**
 * Adds some arrows to the player.
 * If the maximum number of arrows is achieved, no more arrows are added.
 * @param arrows_to_add number of arrows to add
 */
void Equipment::add_arrows(int arrows_to_add) {

  int max_arrows = get_max_arrows();
  int total = get_arrows() + arrows_to_add;

  set_arrows(MIN(total, max_arrows));
}

/**
 * Removes an arrow from the player.
 * If the number of arrows is already zero, nothing happens.
 */
void Equipment::remove_arrow(void) {

  int total = get_arrows() - 1;

  set_arrows(MAX(total, 0));
}

/**
 * Returns whether the player needs arrows.
 * The function returns true if the player has obtained the bow
 * but has 0 arrows left.
 * @return true if the player needs arrows
 */
bool Equipment::needs_arrows(void) {
  return has_inventory_item(ITEM_BOW)
    && get_arrows() == 0;
}

// inventory items

/**
 * Returns whether the player has obtained the specified item.
 * For most of the items, the value returned is always 0 or 1.
 * Some items have several variants (for example a bottle):
 * for such items, the value returned indicates the variant
 * and may be greater than 1.
 * @param item_id id of the item
 * @return a value indicating the possession state of this item.
 */
int Equipment::has_inventory_item(InventoryItemId item_id) {

  int index = Savegame::FIRST_INVENTORY_ITEM + item_id;
  return savegame->get_integer(index);
}

/**
 * Gives an item of the inventory to the player.
 * This is equivalent to give_inventory_item(item_id, 1).
 * @param item_id the item to give
 */
void Equipment::give_inventory_item(InventoryItemId item_id) {
  give_inventory_item(item_id, 1);
}

/**
 * Sets the possession state of an item.
 * @param item_id the item to set
 * @param variant the variant of the item to give to the player,
 * or zero to remove the item
 */
void Equipment::give_inventory_item(InventoryItemId item_id, int variant) {

  int index = Savegame::FIRST_INVENTORY_ITEM + item_id;
  savegame->set_integer(index, variant);
}

/**
 * Removes from the player an item of the inventory.
 * This is equivalent to give_inventory_item(item_id, 0).
 * @param item_id the item to remove
 */
void Equipment::remove_inventory_item(InventoryItemId item_id) {
  give_inventory_item(item_id, 0);
}

/**
 * Gives an empty bottle to the player.
 * This function calls give_inventory_item() with the item id
 * corresponding to the first bottle slot that the player
 * doesn't have yet.
 */
void Equipment::add_bottle(void) {

  if (!has_inventory_item(ITEM_BOTTLE_1)) {
    give_inventory_item(ITEM_BOTTLE_1);
  }
  else if (!has_inventory_item(ITEM_BOTTLE_2)) {
    give_inventory_item(ITEM_BOTTLE_2);
  }
  else if (!has_inventory_item(ITEM_BOTTLE_3)) {
    give_inventory_item(ITEM_BOTTLE_3);
  }
  else if (!has_inventory_item(ITEM_BOTTLE_4)) {
    give_inventory_item(ITEM_BOTTLE_4);
  }
  else {
    DIE("The player already has all bottles");
  }
}

/**
 * Returns whether the player has at least one bottle.
 * @return true if the player has at least one bottle
 */
bool Equipment::has_bottle(void) {

  return has_inventory_item(ITEM_BOTTLE_1)
    || has_inventory_item(ITEM_BOTTLE_2)
    || has_inventory_item(ITEM_BOTTLE_3)
    || has_inventory_item(ITEM_BOTTLE_4);
}

/**
 * Returns whether the player has at least one empty bottle.
 * @return true if the player has at least one empty bottle
 */
bool Equipment::has_empty_bottle(void) {
  return has_bottle_with(Treasure::NONE);
}

/**
 * Returns the id of the first empty bottle.
 * @return the id of the first empty bottle
 */
InventoryItemId Equipment::get_first_empty_bottle(void) {
  return get_first_bottle_with(Treasure::NONE);
}

/**
 * Returns whether the player has at least one bottle having the specified content.
 * @param content the content seeked (must be a valid bottle content or Treasure::NONE)
 * @return true if the player has at least one bottle with this content
 */
bool Equipment::has_bottle_with(Treasure::Content content) {

  int variant;
  if (content == Treasure::NONE) {
    variant = 1;
  }
  else {
    if (content < Treasure::WATER || content > Treasure::FAIRY_IN_BOTTLE) {
      DIE("This content cannot be in a bottle: '" << content << "'");
    }
    variant = content - Treasure::WATER + 2;
  }

  return has_inventory_item(ITEM_BOTTLE_1) == variant
    || has_inventory_item(ITEM_BOTTLE_2) == variant
    || has_inventory_item(ITEM_BOTTLE_3) == variant
    || has_inventory_item(ITEM_BOTTLE_4) == variant;
}

/**
 * Returns the id of the first bottle having the specified content.
 * @param content the content seeked (must be a valid bottle content or Treasure::NONE)
 * @return the id of the first bottle with this content
 */
InventoryItemId Equipment::get_first_bottle_with(Treasure::Content content) {

  int variant;
  if (content == Treasure::NONE) {
    variant = 1;
  }
  else {
    if (content < Treasure::WATER || content > Treasure::FAIRY_IN_BOTTLE) {
      DIE("This content cannot be in a bottle: '" << content << "'");
    }
    variant = content - Treasure::WATER + 2;
  }

  if (has_inventory_item(ITEM_BOTTLE_1) == variant) {
    return ITEM_BOTTLE_1;
  }

  if (has_inventory_item(ITEM_BOTTLE_2) == variant) {
    return ITEM_BOTTLE_2;
  }

  if (has_inventory_item(ITEM_BOTTLE_3) == variant) {
    return ITEM_BOTTLE_3;
  }

  if (has_inventory_item(ITEM_BOTTLE_4) == variant) {
    return ITEM_BOTTLE_4;
  }

  DIE("The player does not have a bottle with content '" << variant << "'");
}

/**
 * Sets the content of a bottle.
 * @param bottle_id item id of the bottle (must be the id of a bottle)
 * @param content content to set (must be a valid bottle content or Treasure::NONE)
 */
void Equipment::set_bottle_content(InventoryItemId bottle_id, Treasure::Content content) {

  if (bottle_id != ITEM_BOTTLE_1 &&
      bottle_id != ITEM_BOTTLE_2 &&
      bottle_id != ITEM_BOTTLE_3 &&
      bottle_id != ITEM_BOTTLE_4) {
    DIE("Invalid bottle id: '" << bottle_id << "'");
  }

  int variant;
  if (content == Treasure::NONE) {
    variant = 1;
  }
  else {
    if (content < Treasure::WATER || content > Treasure::FAIRY_IN_BOTTLE) {
      DIE("This content cannot be in a bottle: '" << content << "'");
    }
    variant = content - Treasure::WATER + 2;
  }
  give_inventory_item(bottle_id, variant);
}

/**
 * Sets a bottle empty.
 * @param bottle_id item id of the bottle to make empty (must be the id of a bottle)
 */
void Equipment::set_bottle_empty(InventoryItemId bottle_id) {
  set_bottle_content(bottle_id, Treasure::NONE);
}

/**
 * Returns the current amount of a specified inventory item.
 * @param item_id id of the item to get
 * @return the player's current amount of this item
 */
int Equipment::get_inventory_item_amount(InventoryItemId item_id) {

  if (item_id == ITEM_BOMBS) {
    return get_bombs();
  }

  if (item_id == ITEM_BOW) {
    return get_arrows();
  }

  int counter_index = InventoryItem::get_counter_index(item_id);

  return savegame->get_integer(counter_index);
}

/**
 * Sets the current amount of a specified item.
 * The program exits with an error message if the given number
 * is not valid.
 * @param item_id id of the item to set
 * @param amount the new amount
 */
void Equipment::set_inventory_item_amount(InventoryItemId item_id, int amount) {

  if (item_id == ITEM_BOMBS) {
    set_bombs(amount);
  }
  else if (item_id == ITEM_BOW) {
    set_arrows(amount);
  }
  else {

    int counter_index = InventoryItem::get_counter_index(item_id);

    // the item can be the fire stones, the apples,
    // the pains au chocolat or the croissants
    int max = (item_id == ITEM_FIRE_STONES) ? 3 : 10;

    if (amount < 0 || amount > max) {
      DIE("Illegal amount for item " << item_id << ": " << amount);
    }

    savegame->set_integer(counter_index, amount);
  }
}

/**
 * Adds an amount of a specified inventory item to the player.
 * If the maximum amount is achieved, no more items are added.
 * @param item_id id of the item to set
 * @param amount_to_add the amount to add
 */
void Equipment::add_inventory_item_amount(InventoryItemId item_id, int amount_to_add) {

  if (item_id == ITEM_BOMBS) {
    add_bombs(amount_to_add);
  }
  else if (item_id == ITEM_BOW) {
    add_arrows(amount_to_add);
  }
  else {
    int total = get_inventory_item_amount(item_id) + amount_to_add;

    if (item_id == ITEM_FIRE_STONES && amount_to_add != 1) {
      DIE("Only one fire stone can be added");
    }

    set_inventory_item_amount(item_id, MIN(total, 10));
  }
}

/**
 * Removes an amount of a specified inventory item to the player.
 * If the amount reaches zero, no more items are removed.
 * @param item_id id of the item to set
 * @param amount_to_remove the amount to remove
 */
void Equipment::remove_inventory_item_amount(InventoryItemId item_id, int amount_to_remove) {

  if (item_id == ITEM_FIRE_STONES) {
    DIE("Cannot remove a fire stone");
  }
  else {
    int total = get_inventory_item_amount(item_id) - 1;
    set_inventory_item_amount(item_id, MAX(total, 0));
  }
}

/**
 * Returns the item currently assigned to a slot.
 * @param slot slot of the item to get (0 for X or 1 for V)
 * @return the item currently assigned to this slot (may be NONE)
 */
InventoryItemId Equipment::get_item_assigned(int slot) {
  int index = Savegame::ITEM_SLOT_0 + slot;
  return (InventoryItemId) savegame->get_integer(index);
}

/**
 * Assigns an item to a slot.
 * The program exits with an error message if the specified item
 * cannot be equiped or if the player does not have it
 * @param slot slot to set (0 for X or 1 for V)
 * @param item_id the item to assign to this slot
 */
void Equipment::set_item_assigned(int slot, InventoryItemId item_id) {

  if (!has_inventory_item(item_id)) {
    DIE("Cannot assign item " << item_id << " because the player does not have it");
  }

  if (!InventoryItem::is_attributable(item_id)) {
    DIE("Cannot assign item " << item_id << " because it is not attributable");
  }

  int index = Savegame::ITEM_SLOT_0 + slot;
  savegame->set_integer(index, item_id);
}

/**
 * Returns whether the player has got the world map.
 * @return true if the player has got the world map
 */
bool Equipment::has_world_map(void) {
  return savegame->get_integer(Savegame::WORLD_MAP) != 0;
}

/**
 * Gives the world map to the player.
 */
void Equipment::add_world_map(void) {
  savegame->set_integer(Savegame::WORLD_MAP, 1);
}

// small keys

/**
 * Returns whether the small keys are enabled in the current map.
 * @return true if the small keys are enabled in the current map
 */
bool Equipment::are_small_keys_enabled(void) {
  return zsdx->game->get_current_map()->has_small_keys();
}

/**
 * Returns the savegame variable where the number of small keys
 * is stored for the current map.
 * Stops with an error message if the small keys are not enabled in the current map.
 */
int Equipment::get_small_keys_variable(void) {

  if (!are_small_keys_enabled()) {
    DIE("The small keys are not enabled on this map");
  }

  return zsdx->game->get_current_map()->get_small_keys_variable();
}

/**
 * Returns whether the player has got at least one small key in the current map.
 * Stops with an error message if the small keys are not enabled in the current map.
 * @return true if the player has got at least one small key
 */
bool Equipment::has_small_key(void) {
  return get_small_keys() > 0;
}

/**
 * Returns the current number of small keys of the player in the current map.
 * Stops with an error message if the small keys are not enabled in the current map.
 * @return the current number of small keys
 */
int Equipment::get_small_keys(void) {
  int index = get_small_keys_variable();
  return savegame->get_integer(index);
}

/**
 * Adds a small key to the player in the current map.
 */
void Equipment::add_small_key(void) {

  int index = get_small_keys_variable();
  savegame->set_integer(index, get_small_keys() + 1);
}

/**
 * Removes a small key from the player in the current dungeon.
 * Stops with an error message if the player has no more small keys.
 */
void Equipment::remove_small_key(void) {

  if (!has_small_key()) {
    DIE("The player has no small keys");
  }

  int index = get_small_keys_variable();
  savegame->set_integer(index, get_small_keys() - 1);
}

// ability to lift items

/**
 * Returns whether the player can lift the specified weight.
 * - If the weight is 0, true is always returned.
 * - If the weight is 1, the function returns true if the player has got the iron glove.
 * - If the weight is 2, the function returns true if the player has got the golden glove.
 * @param weight the weight of an item (0 to 2)
 * @return true if the player can lift this weight.
 */
bool Equipment::can_lift(int weight) {
  return has_inventory_item(ITEM_GLOVE) >= weight;
}
