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
#include "Equipment.h"
#include "Game.h"
#include "Savegame.h"
#include "DialogBox.h"
#include "InventoryItem.h"
#include "ItemProperties.h"
#include "Map.h"
#include "lua/ItemScript.h"
#include "lowlevel/System.h"
#include "lowlevel/IniFile.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Constructor.
 * @param savegame the savegame to encapsulate
 */
Equipment::Equipment(Savegame &savegame):
  savegame(savegame), game(NULL), magic_decrease_delay(0) {

  // load the equipment specification from items.dat
  IniFile ini("items.dat", IniFile::READ);
  ini.start_group_iteration();
  while (ini.has_more_groups()) {

    item_properties[ini.get_group()] = new ItemProperties(*this, ini);
    ini.next_group();
  }
}

/**
 * @brief Destructor.
 */
Equipment::~Equipment() {

  {
    std::map<std::string, ItemProperties*>::const_iterator it;
    for (it = item_properties.begin(); it != item_properties.end(); it++) {
      delete it->second;
    }
  }

  {
    std::map<std::string, ItemScript*>::const_iterator it;
    for (it = item_scripts.begin(); it != item_scripts.end(); it++) {
      delete it->second;
    }
  }
}

/**
 * @brief Returns the script of an item.
 * @param item_name name of the item
 * @return the corresponding script
 */
ItemScript& Equipment::get_item_script(const std::string &item_name) {

  Debug::assert(item_scripts.count(item_name) != 0,
                StringConcat() << "Cannot find item script '" << item_name << "'");

  return *item_scripts[item_name];
}

/**
 * @brief Sets the current game.
 * @param game the game
 */
void Equipment::set_game(Game &game) {

  Debug::assert(this->game == NULL, "The game is already set");

  this->game = &game;

  // create the scripts
  std::map<std::string, ItemProperties*>::const_iterator it;
  for (it = item_properties.begin(); it != item_properties.end(); it++) {
    ItemProperties *properties = it->second;
    item_scripts[properties->get_name()] = new ItemScript(game, *properties);
  }
}

/**
 * @brief This function is be called repeatedly by the game.
 *
 * Most of the time, there is nothing to update in this class.
 * The only elements updated here are some dynamic things
 * such as equipment-related animations or dialogs.
 */
void Equipment::update() {

  if (game == NULL) {
    // nothing dynamic when there is no game
    return;
  }

  // check if the game is suspended
  bool game_suspended = game->is_suspended();
  if (suspended != game_suspended) {
    set_suspended(game_suspended);
  }

  if (!suspended) {

    // update the decreasing magic bar
    if (magic_decrease_delay != 0
	&& System::now() > next_magic_decrease_date) {

      remove_magic(1);

      if (get_magic() > 0) {
	next_magic_decrease_date += magic_decrease_delay;
      }
      else {
	stop_removing_magic();
      }
    }
  }

  // update the item scripts
  std::map<std::string, ItemScript*>::iterator it;
  for (it = item_scripts.begin(); it != item_scripts.end(); it++) {
    it->second->update();
  }
}

/**
 * @brief This functions is called when the game is suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void Equipment::set_suspended(bool suspended) {

  // notify the item scripts
  std::map<std::string, ItemScript*>::iterator it;
  for (it = item_scripts.begin(); it != item_scripts.end(); it++) {
    it->second->set_suspended(suspended);
  }
}

// money

/**
 * @brief Returns the maximum amount of money of the player.
 * @return the player's maximum number of money
 */
int Equipment::get_max_money() {
  return savegame.get_integer(Savegame::MAX_MONEY);
}

/**
 * @brief Sets the maximum amount of money of the player.
 * @param max_money the player's maximum amount number of money
 */
void Equipment::set_max_money(int max_money) {

  Debug::assert(max_money > 0, StringConcat() << "Illegal maximum amount of money: " << max_money);

  savegame.set_integer(Savegame::MAX_MONEY, max_money);
}

/**
 * @brief Returns the player's current amount of money
 * @return the player's current amount of money
 */
int Equipment::get_money() {
  return savegame.get_integer(Savegame::CURRENT_MONEY);
}


/**
 * @brief Sets the player's current number of money.
 *
 * Exits with an error message if the given amount of money is not valid.
 *
 * @param money the player's new amount of money
 */
void Equipment::set_money(int money) {

  Debug::assert(money >= 0 && money <= get_max_money(), StringConcat() << "Illegal amount of money: " << money);

  savegame.set_integer(Savegame::CURRENT_MONEY, money);
}

/**
 * @brief Adds some money to the player.
 *
 * If the maximum amount of money is reached, no more money is added.
 * 
 * @param money_to_add amount of money to add
 */
void Equipment::add_money(int money_to_add) {

  int max_money = get_max_money();
  int total = get_money() + money_to_add;

  set_money(std::min(total, max_money));
}

/**
 * @brief Removes some money from the player.
 *
 * If the money reaches zero, no more money is removed.
 *
 * @param money_to_remove amount of money to remove
 */
void Equipment::remove_money(int money_to_remove) {

  int total = get_money() - money_to_remove;

  set_money(std::max(total, 0));
}

// life

/**
 * @brief Returns the maximum level of life of the player.
 * @return the player's maximum level of life
 */
int Equipment::get_max_life() {
  return savegame.get_integer(Savegame::MAX_LIFE);
}

/**
 * @brief Sets the maximum level of life of the player.
 *
 * The program exits with an error message if the specified maximum
 * life is not valid.
 * 
 * @param max_life the player's maximum life
 */
void Equipment::set_max_life(int max_life) {

  Debug::assert(max_life > 0, StringConcat() << "Illegal maximum life: " << max_life);

  savegame.set_integer(Savegame::MAX_LIFE, max_life);
}

/**
 * @brief Returns the current level of life of the player.
 * @return the player's current life
 */
int Equipment::get_life() {
  return savegame.get_integer(Savegame::CURRENT_LIFE);
}

/**
 * @brief Sets the current life of the player.
 *
 * The program exits with an error message if the specified life.
 *
 * @param life the player's new level of life
 */
void Equipment::set_life(int life) {

  Debug::assert(life > 0 && life <= get_max_life(), StringConcat() << "Illegal level of life: " << life);

  savegame.set_integer(Savegame::CURRENT_LIFE, life);
}

/**
 * @brief Gives some life to the player.
 *
 * If the maximum life is reached, no more life is added.
 *
 * @param life_to_add level of life to add
 */
void Equipment::add_life(int life_to_add) {

  int total = get_life() + life_to_add;

  set_life(std::min(total, get_max_life()));
}

/**
 * @brief Removes some life from the player.
 * @param life_to_remove amount of life to remove
 */
void Equipment::remove_life(int life_to_remove) {

  int total = get_life() - life_to_remove;

  set_life(std::max(total, 0));
}

/**
 * @brief Restores all the life.
 */
void Equipment::restore_all_life() {
  set_life(get_max_life());
}

// magic

/**
 * @brief Returns the maximum number of magic points.
 * @return the maximum level of magic
 */
int Equipment::get_max_magic() {
  return savegame.get_integer(Savegame::MAX_MAGIC);
}

/**
 * @brief Sets the maximum number of magic points.
 *
 * Exits with an error message if the value specified
 * if not valid.
 * 
 * @param max_magic the maximum level of magic
 */
void Equipment::set_max_magic(int max_magic) {

  Debug::assert(max_magic >= 0, StringConcat() << "Illegal maximum number of magic points: " << max_magic);

  savegame.set_integer(Savegame::MAX_MAGIC, max_magic);

  restore_all_magic();
}

/**
 * @brief Returns the current number of magic points of the player.
 * @return the player's current number of magic points
 */
int Equipment::get_magic() {
  return savegame.get_integer(Savegame::CURRENT_MAGIC);
}

/**
 * @brief Sets the current number of magic points of the player.
 *
 * The program exits with an error message if the given value
 * is not valid.
 * 
 * @param magic the player's new number of magic points
 */
void Equipment::set_magic(int magic) {

  Debug::assert(magic > 0 && magic <= get_max_magic(), StringConcat() << "Illegal number of magic points: " << magic);

  savegame.set_integer(Savegame::CURRENT_MAGIC, magic);
}

/**
 * @brief Adds some magic points to the player.
 *
 * If the maximum value is reached, no more magic points are added.
 * 
 * @param magic_to_add number of magic points to add
 */
void Equipment::add_magic(int magic_to_add) {

  int total = get_magic() + magic_to_add;

  set_magic(std::min(total, get_max_magic()));
}

/**
 * @brief Removes some magic points from the player.
 *
 * If the number of magic points reaches zero, no more magic points
 * are removed.
 *
 * @param magic_to_remove number of magic poits to remove
 */
void Equipment::remove_magic(int magic_to_remove) {

  int total = get_magic() - magic_to_remove;

  set_magic(std::max(total, 0));
}

/**
 * @brief Restores all magic points.
 */
void Equipment::restore_all_magic() {
  set_magic(get_max_magic());
}

/**
 * @brief Returns whether the magic bar is decreasing continuously.
 * @return true if the magic bar is decreasing, false otherwise
 */
bool Equipment::is_magic_decreasing() {
  return this->magic_decrease_delay != 0;
}

/**
 * @brief Starts removing magic continuously.
 * @param delay delay in miliseconds between two decreases
 */
void Equipment::start_removing_magic(uint32_t delay) {

  Debug::assert(delay > 0, StringConcat() << "Illegal magic bar decrease delay: " << delay);

  if (get_magic() > 0) {
    this->magic_decrease_delay = delay;
    this->next_magic_decrease_date = System::now();

    // the magic points will be removed by the update() function
  }
}

/**
 * @brief Stops removing magic continuously.
 */
void Equipment::stop_removing_magic() {
  this->magic_decrease_delay = 0;
}

// saved items

/**
 * @brief Returns the static properties of the specified item.
 * @param item_name name of the item to get
 */
ItemProperties& Equipment::get_item_properties(const std::string &item_name) {

  ItemProperties *properties = item_properties[item_name];
  Debug::assert(properties != NULL, StringConcat() << "Cannot find item with name '" << item_name << "'");
  return *properties;
}

/**
 * @brief Returns whether the player has obtained the specified saved item.
 *
 * This function can return true only for items that are saved.
 * This function is equivalent to get_item_variant(item_name) != 0.
 *
 * @param item_name name of the item
 * @return true if the player has this item.
 */
bool Equipment::has_item(const std::string &item_name) {

  return get_item_variant(item_name) != 0;
}

/**
 * @brief Returns the possession state of the specified saved item.
 *
 * This function makes sense only for items whose possession state is saved.
 *
 * @param item_name name of the item
 * @return the variant of this item that the player has
 */
int Equipment::get_item_variant(const std::string &item_name) {

  int index = get_item_properties(item_name).get_savegame_variable();
  if (index == -1) {
    return 0;
  }

  return savegame.get_integer(index);
}

/**
 * @brief Sets the possession state of a saved item.
 * @param item_name the item to set
 * @param variant the variant of the item to give to the player,
 * or zero to remove the item
 */
void Equipment::set_item_variant(const std::string &item_name, int variant) {

  ItemProperties &properties = get_item_properties(item_name);
  int index = properties.get_savegame_variable();
  Debug::assert(index != -1, StringConcat() << "The item '" << item_name << "' is not saved");
  Debug::assert(variant >= 0 && variant <= properties.get_nb_variants(),
      StringConcat() << "Invalid variant '" << variant << "' for item '" << item_name);

  // set the possession state in the savegame
  savegame.set_integer(index, variant);

  // if we are removing the item, unassign it
  if (variant == 0) {
    if (get_item_assigned(0) == item_name) {
      set_item_assigned(0, "");
    }
    else if (get_item_assigned(1) == item_name) {
      set_item_assigned(1, "");
    }
  }
}

/**
 * @brief Removes a saved item from the player.
 *
 * This is equivalent to set_item_variant(item_name, 0).
 *
 * @param item_name the item to remove
 */
void Equipment::remove_item(const std::string &item_name) {
  set_item_variant(item_name, 0);
}

/**
 * @brief Returns the current value of the counter associated to an item.
 * @param item_name name of the item to get
 * @return the player's current amount of this item
 */
int Equipment::get_item_amount(const std::string &item_name) {

  int counter_index = get_item_properties(item_name).get_counter_savegame_variable();
  Debug::assert(counter_index != -1, StringConcat() << "No amount for item '" << item_name << "'");

  return savegame.get_integer(counter_index);
}

/**
 * @brief Sets the current value of the counter of a specified item.
 *
 * The program exits with an error message if the specified amount
 * is not valid.
 *
 * @param item_name name of the item to set
 * @param amount the new amount
 */
void Equipment::set_item_amount(const std::string &item_name, int amount) {

  int counter_index = get_item_properties(item_name).get_counter_savegame_variable();
  int max = get_item_maximum(item_name);

  Debug::assert(counter_index != -1, StringConcat() << "No amount for item '" << item_name << "'");
  Debug::assert(amount >= 0 && amount <= max, StringConcat() << "Illegal amount for item '" << item_name << "': " << amount);

  savegame.set_integer(counter_index, amount);

  get_item_script(item_name).event_amount_changed(amount);
}

/**
 * @brief Adds an amount of a specified item to the player.
 *
 * If the maximum amount is reached, no more items are added.
 * 
 * @param item_name name of the item to set
 * @param amount_to_add the amount to add
 */
void Equipment::add_item_amount(const std::string &item_name, int amount_to_add) {

  int total = get_item_amount(item_name) + amount_to_add;
  set_item_amount(item_name, std::min(total, get_item_maximum(item_name)));
}

/**
 * @brief Removes an amount of a specified item to the player.
 *
 * If the amount reaches zero, no more items are removed.
 * 
 * @param item_name name of the item to set
 * @param amount_to_remove the amount to remove
 */
void Equipment::remove_item_amount(const std::string &item_name, int amount_to_remove) {

  int total = get_item_amount(item_name) - amount_to_remove;
  set_item_amount(item_name, std::max(total, 0));
}

/**
 * @brief Returns the maximum amount value of the specified item.
 * @param item_name name of an item 
 * @return the maximum amount value of this item
 */
int Equipment::get_item_maximum(const std::string &item_name) {

  // find the maximum as a fixed value or a value that depends on another item
  int maximum = 0;

  ItemProperties &properties = get_item_properties(item_name);
  int fixed_limit = properties.get_fixed_limit();
  if (fixed_limit != 0) {
    maximum = fixed_limit;
  }
  else {
    const std::string &item_limiting = properties.get_item_limiting();
    int item_limiting_variant = get_item_variant(item_limiting);
    maximum = get_item_properties(item_limiting).get_amount(item_limiting_variant);
  }

  Debug::assert(maximum != 0, StringConcat() << "No maximum amount for item '" << item_name << "'");

  return maximum;
}

/**
 * @brief Returns whether the player cannot get more of the specified item.
 * @param item_name name of an item
 * @return true if the player has the maximum amount of this item
 */
bool Equipment::has_item_maximum(const std::string &item_name) {

  bool result;
  const std::string &item_counter_changed = get_item_properties(item_name).get_item_counter_changed();
  if (item_counter_changed.size() == 0) {
    result = false;
  }

  else if (item_counter_changed == "life") {
    result = (get_life() >= get_max_life());
  }

  else if (item_counter_changed == "money") {
    result = (get_money() >= get_max_money());
  }

  else if (item_counter_changed == "magic") {
    result = (get_magic() >= get_max_magic());
  }
  
  else if (item_counter_changed == "small_keys") {
    result = false;
  }

  else {
    result = get_item_amount(item_counter_changed) >= get_item_maximum(item_counter_changed);
  }

  return result;
}

/**
 * @brief Chooses randomly the name and variant of an item, with respect
 * to the probabilities indicated in the file items.dat.
 *
 * This function may return an item that the player is not authorized to have yet.
 *
 * @param item_name the name of an item randomly chosen (possibly "_none")
 * @param variant variant of this item
 */
void Equipment::get_random_item(std::string &item_name, int &variant) {
  item_name = "_none";
  variant = 1;
  // TODO
}

/**
 * @brief Returns the item currently assigned to a slot.
 * @param slot slot of the item to get (0 for X or 1 for V)
 * @return name of the item currently assigned to this slot, or an empty string
 */
const std::string Equipment::get_item_assigned(int slot) {

  int index = Savegame::ITEM_SLOT_0 + slot;
  return savegame.get_string(index);
}

/**
 * @brief Assigns an item to a slot.
 *
 * The program exits with an error message if the specified item
 * cannot be equiped or if the player does not have it.
 * 
 * @param slot slot to set (0 for X or 1 for V)
 * @param item_name the item to assign to this slot (may be an empty string)
 */
void Equipment::set_item_assigned(int slot, const std::string &item_name) {

  if (item_name.size() != 0) {
    Debug::assert(has_item(item_name), StringConcat() << "Cannot assign item '" << item_name << "' because the player does not have it");
    Debug::assert(get_item_properties(item_name).can_be_assigned(), StringConcat() << "The item '" << item_name << "' cannot be assigned");
  }

  int index = Savegame::ITEM_SLOT_0 + slot;
  savegame.set_string(index, item_name);
}

/**
 * @brief Returns the slot (0 or 1) where the specified item is currently assigned.
 * @param item_name name of the item
 * @return the slot of this item, or -1 if this item is not assigned
 */
int Equipment::get_item_slot(const std::string &item_name) {
  
  if (get_item_assigned(0) == item_name) {
    return 0;
  }

  if (get_item_assigned(1) == item_name) {
    return 1;
  }

  return -1;
}

// small keys

/**
 * @brief Returns whether the small keys are enabled in the current map.
 * @return true if the small keys are enabled in the current map
 */
bool Equipment::are_small_keys_enabled() {
  return game->get_current_map().has_small_keys();
}

/**
 * @brief Returns the savegame variable where the number of small keys
 * is stored for the current map.
 *
 * Stops with an error message if the small keys are not enabled in the current map.
 */
int Equipment::get_small_keys_variable() {

  Debug::assert(are_small_keys_enabled(), "The small keys are not enabled on this map");

  return game->get_current_map().get_small_keys_variable();
}

/**
 * @brief Returns whether the player has got at least one small key in the current map.
 *
 * Stops with an error message if the small keys are not enabled in the current map.
 * 
 * @return true if the player has got at least one small key
 */
bool Equipment::has_small_key() {
  return get_small_keys() > 0;
}

/**
 * @brief Returns the current number of small keys of the player in the current map.
 *
 * Stops with an error message if the small keys are not enabled in the current map.
 * 
 * @return the current number of small keys
 */
int Equipment::get_small_keys() {
  int index = get_small_keys_variable();
  return savegame.get_integer(index);
}

/**
 * @brief Adds one or several small keys to the player in the current map.
 * @param amount_to_add number of small keys to add
 */
void Equipment::add_small_keys(int amount_to_add) {

  int index = get_small_keys_variable();
  savegame.set_integer(index, get_small_keys() + amount_to_add);
}

/**
 * @brief Removes a small key from the player in the current dungeon.
 *
 * Stops with an error message if the player has no more small keys.
 */
void Equipment::remove_small_key() {

  Debug::assert(has_small_key(), "The player has no small keys");

  int index = get_small_keys_variable();
  savegame.set_integer(index, get_small_keys() - 1);
}

// abilities

/**
 * @brief Returns the index of the savegame variable that stores the specified ability.
 * @param ability_name name of the ability
 * @return the index of the savegame variable that stores this ability
 */
int Equipment::get_ability_savegame_variable(const std::string &ability_name) {

  int index = -1;

  if (ability_name == "tunic") {
    index = Savegame::ABILITY_TUNIC;
  }
  else if (ability_name == "sword") {
    index = Savegame::ABILITY_SWORD;
  }
  else if (ability_name == "shield") {
    index = Savegame::ABILITY_SHIELD;
  }
  else if (ability_name == "lift") {
    index = Savegame::ABILITY_LIFT;
  }
  else if (ability_name == "swim") {
    index = Savegame::ABILITY_SWIM;
  }
  else if (ability_name == "see_in_dark") {
    index = Savegame::ABILITY_SEE_IN_DARK;
  }
  else if (ability_name == "detect_weak_walls") {
    index = Savegame::ABILITY_DETECT_WEAK_WALLS;
  }
  else if (ability_name == "see_outside_world_minimap") {
    index = Savegame::ABILITY_SEE_OUTSIDE_WORLD_MINIMAP;
  }
  else if (ability_name == "see_dungeon_minimap_rooms") {
    index = Savegame::DUNGEON_1_ABILITY_SEE_MINIMAP_ROOMS + 10 * (get_current_dungeon() - 1);
  }
  else if (ability_name == "see_dungeon_minimap_elements") {
    index = Savegame::DUNGEON_1_ABILITY_SEE_MINIMAP_ELEMENTS + 10 * (get_current_dungeon() - 1);
  }
  else if (ability_name == "open_dungeon_big_locks") {
    index = Savegame::DUNGEON_1_ABILITY_OPEN_BIG_LOCKS + 10 * (get_current_dungeon() - 1);
  }
  else if (ability_name == "open_dungeon_boss_lock") {
    index = Savegame::DUNGEON_1_ABILITY_OPEN_BOSS_LOCK + 10 * (get_current_dungeon() - 1);    
  }

  Debug::assert(index != -1, StringConcat() << "Unknown ability '" << ability_name << "'");

  return index;
}

/**
 * @brief Returns whether the player has at least the specified level of an ability.
 * @param ability_name the ability to get
 * @param level the minimum level to check
 * @return true if the player has at least this level of the ability
 */
bool Equipment::has_ability(const std::string &ability_name, int level) {
  return get_ability(ability_name) >= level;
}

/**
 * @brief Returns the level of the specified ability.
 * @param ability_name the ability to get
 * @return the level of this ability
 */
int Equipment::get_ability(const std::string &ability_name) {
  return savegame.get_integer(get_ability_savegame_variable(ability_name));
}

/**
 * @brief Sets the level of the specified ability.
 * @param ability_name the ability to set
 * @param level the level of this ability
 */
void Equipment::set_ability(const std::string &ability_name, int level) {
  savegame.set_integer(get_ability_savegame_variable(ability_name), level);
}

/**
 * @brief This function is called when the player used an ability.
 * @param ability_name the ability used
 */
void Equipment::use_ability(const std::string &ability_name) {
  // TODO notify the script
}

// dungeons

/**
 * @brief Returns the current dungeon number.
 *
 * The dungeon number returned is between 1 and 40.
 * If the player is not in a dungeon, 0 is returned.
 *
 * @return the number of the current dungeon between 1 and 40, or 0
 */
int Equipment::get_current_dungeon() {

  if (!game->is_in_dungeon()) {
    return 0;
  }

  return game->get_current_map().get_world_number();
}

/**
 * @brief Returns whether the player has finished the specified dungeon.
 * @param dungeon a dungeon
 * @return true if the specified dungeon is finished
 */
bool Equipment::is_dungeon_finished(int dungeon) {

  int index = Savegame::DUNGEON_1_FINISHED + 10 * (dungeon - 1);
  return savegame.get_integer(index) > 0;
}

/**
 * @brief Returns whether the player has finished the current dungeon.
 * @return true if the current dungeon is finished
 */
bool Equipment::is_dungeon_finished() {
  return is_dungeon_finished(get_current_dungeon());
}

/**
 * @brief Sets the specified dungeon as finished.
 * @param dungeon a dungeon
 */
void Equipment::set_dungeon_finished(int dungeon) {

  int index = Savegame::DUNGEON_1_FINISHED + 10 * (dungeon - 1);
  savegame.set_integer(index, 1);
}

// obtaining items

/**
 * @brief Gives to the player the initial variant of each item saved,
 * according to the file items.dat.
 * 
 * This function is called when the savegame is created.
 */
void Equipment::set_initial_items() {

  std::map<std::string, ItemProperties*>::const_iterator it;

  for (it = item_properties.begin(); it != item_properties.end(); it++) {

    ItemProperties *properties = it->second;
    int initial_variant = properties->get_initial_variant();
    if (initial_variant != 0) {
      add_item(it->first, initial_variant);
    }
  }
}

/**
 * @brief Retruns whether the player is authorized to have the specified item.
 *
 * The player is not authorized to receive an item if
 * the item increases a counter whose maximum value is currently zero
 * (for example receiving magic without magic bar, receiving bombs without bomb bag, etc.)
 *
 * @param item_name name of the item to test
 * @param variant variant of the item
 * @return true if the player can have this item
 */
bool Equipment::can_receive_item(const std::string &item_name, int variant) {

  bool authorized = true;

  ItemProperties &properties = get_item_properties(item_name);

  // see if obtaining this item increases the counter of another item
  const std::string &item_counter_changed = properties.get_item_counter_changed();
  if (item_counter_changed.size() > 0) {

    // consider built-in counters
    if (item_counter_changed == "magic") {
      authorized = get_max_magic() > 0;
    }
    else if (item_counter_changed != "life"
	    && item_counter_changed != "money"
	    && item_counter_changed != "small_keys") { // general case
      // check that the player has unlocked the counter of the item to increase
      const std::string &item_limiting = get_item_properties(item_counter_changed).get_item_limiting();
      authorized = (item_limiting.size() == 0) || has_item(item_limiting);
    }
  }

  return authorized;
}

/**
 * @brief Adds an item to the equipment.
 *
 * This function can be called with any kind of item and it makes the
 * appropriate modifications on equipment according to the item description in items.dat.
 *
 * @param item_name name of the item to add
 * @param variant variant of this item
 */
void Equipment::add_item(const std::string &item_name, int variant) {

  ItemProperties &properties = get_item_properties(item_name);

  if (properties.is_saved()) {

    // the item is saved
    set_item_variant(item_name, variant);

    if (properties.has_counter()) {
      // the item has a counter

      // if another item acts as a limit for the counter of this item
      const std::string &item_limiting = properties.get_item_limiting();
      if (item_limiting.size() > 0) {

	// make sure we have at least the first variant of that item
	if (!has_item(item_limiting)) {

	  // for example, we give the bomb counter: also give the bomb bag
	  set_item_variant(item_limiting, 1);
	}
      }

      // set the counter at its maximum value
      set_item_amount(item_name, get_item_maximum(item_name));
    }

    // see if this item acts as a limit for another item
    const std::string &item_limited = properties.get_item_limited();
    if (item_limited.size() > 0) {

      int maximum = properties.get_amount(variant);

      // consider built-in counters
      if (item_limited == "life") {
	set_max_life(maximum);
	restore_all_life();
      }
      else if (item_limited == "money") {
	set_max_money(maximum);
      }
      else if (item_limited == "magic") {
	set_max_magic(maximum);
	restore_all_magic();
      }
      else { // general case

	// make sure we have the other item
	if (!has_item(item_limited)) {

	  // for example, we give the bomb bag: also give the bomb counter
	  set_item_variant(item_limited, 1);
	}

	// make sure the other item has its new maximum value
	set_item_amount(item_limited, maximum);
      }
    }
  }

  else {
    // now, see if obtaining this item changes the counter of another item
    const std::string &item_counter_changed = properties.get_item_counter_changed();
    if (item_counter_changed.size() > 0) {

      int amount = properties.get_amount(variant);

      // consider built-in counters
      if (item_counter_changed == "life") {
	add_life(amount);
      }
      else if (item_counter_changed == "money") {
	add_money(amount);
      }
      else if (item_counter_changed == "magic") {
	add_magic(amount);
      }
      else if (item_counter_changed == "small_keys") {
	add_small_keys(amount);
      }
      else { // general case

	// make sure the player has the item to increase
	if (!has_item(item_counter_changed)) {
	  set_item_variant(item_counter_changed, 1);
	}

	// for example, we give some bombs: increase the bomb counter
	add_item_amount(item_counter_changed, amount);
      }
    }
  }

  // TODO notify the script instead of this quest-specific code
  if (item_name == "tunic"
      || item_name == "sword"
      || item_name == "shield") {
    set_ability(item_name, variant);
  }
}

