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
#include "Equipment.h"
#include "Game.h"
#include "Savegame.h"
#include "DialogBox.h"
#include "EquipmentItem.h"
#include "Map.h"
#include "entities/Hero.h"
#include "lua/LuaContext.h"
#include "lowlevel/System.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Random.h"

/**
 * @brief Constructor.
 * @param savegame The savegame to encapsulate.
 */
Equipment::Equipment(Savegame& savegame):
  savegame(savegame),
  suspended(true) {

}

/**
 * @brief Destructor.
 */
Equipment::~Equipment() {

  std::map<std::string, EquipmentItem*>::const_iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    EquipmentItem* item = it->second;
    item->decrement_refcount();
    if (item->get_refcount() == 0) {
      delete item;
    }
  }
}

/**
 * @brief Returns the savegame represented by this equipment object.
 * @return The savegame.
 */
Savegame& Equipment::get_savegame() {
  return savegame;
}

/**
 * @brief If this equipment object is currently running in a game, return that game.
 * @return A game or NULL.
 */
Game* Equipment::get_game() {
  return savegame.get_game();
}

/**
 * @brief Notifies the equipment that the game has just started.
 */
void Equipment::notify_game_started() {
}

/**
 * @brief Notifies the equipment that the game has is finished.
 */
void Equipment::notify_game_finished() {

  // The equipment items will disappear: notify them.
  std::map<std::string, EquipmentItem*>::const_iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    it->second->exit();
  }
}

/**
 * @brief Notifies the equipment system has another map has just been started.
 * @param map the new current map
 */
void Equipment::notify_map_changed(Map& map) {

  // Notify the items.
  std::map<std::string, EquipmentItem*>::const_iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    it->second->notify_map_changed(map);
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

  Game* game = savegame.get_game();
  if (game == NULL) {
    // nothing dynamic when there is no game
    return;
  }

  // check if the game is suspended
  bool game_suspended = game->is_suspended();
  if (suspended != game_suspended) {
    set_suspended(game_suspended);
  }

  // update the item scripts
  std::map<std::string, EquipmentItem*>::const_iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    it->second->update();
  }
}

/**
 * @brief This function is called when the game is suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void Equipment::set_suspended(bool suspended) {

  this->suspended = suspended;

  // notify the item scripts
  std::map<std::string, EquipmentItem*>::const_iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    it->second->set_suspended(suspended);
  }
}

// money

/**
 * @brief Returns the maximum amount of money of the player.
 * @return the player's maximum number of money
 */
int Equipment::get_max_money() {
  return savegame.get_integer(Savegame::KEY_MAX_MONEY);
}

/**
 * @brief Sets the maximum amount of money of the player.
 * @param max_money the player's maximum amount number of money
 */
void Equipment::set_max_money(int max_money) {

  Debug::check_assertion(max_money > 0, StringConcat()
      << "Illegal maximum amount of money: " << max_money);

  savegame.set_integer(Savegame::KEY_MAX_MONEY, max_money);
}

/**
 * @brief Returns the player's current amount of money
 * @return the player's current amount of money
 */
int Equipment::get_money() {
  return savegame.get_integer(Savegame::KEY_CURRENT_MONEY);
}


/**
 * @brief Sets the player's current amount of money.
 *
 * If the amount is lower than zero, it is replaced by zero.
 * If the amount is greater than get_max_money(), it is replaced by that value.
 *
 * @param money The player's new amount of money.
 */
void Equipment::set_money(int money) {

  money = std::max(0, std::min(get_max_money(), money));
  savegame.set_integer(Savegame::KEY_CURRENT_MONEY, money);
}

/**
 * @brief Adds some money to the player.
 *
 * If the maximum amount of money is reached, no more money is added.
 * 
 * @param money_to_add amount of money to add
 */
void Equipment::add_money(int money_to_add) {

  set_money(get_money() + money_to_add);
}

/**
 * @brief Removes some money from the player.
 *
 * If the money reaches zero, no more money is removed.
 *
 * @param money_to_remove amount of money to remove
 */
void Equipment::remove_money(int money_to_remove) {

  set_money(get_money() - money_to_remove);
}

// life

/**
 * @brief Returns the maximum level of life of the player.
 * @return the player's maximum level of life
 */
int Equipment::get_max_life() {
  return savegame.get_integer(Savegame::KEY_MAX_LIFE);
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

  Debug::check_assertion(max_life > 0, StringConcat()
      << "Illegal maximum life: " << max_life);

  savegame.set_integer(Savegame::KEY_MAX_LIFE, max_life);
}

/**
 * @brief Returns the current level of life of the player.
 * @return the player's current life
 */
int Equipment::get_life() {
  return savegame.get_integer(Savegame::KEY_CURRENT_LIFE);
}

/**
 * @brief Sets the current life of the player.
 *
 * If the life is lower than zero, it is replaced by zero.
 * If the life is greater than get_max_life(), it is replaced by that value.
 *
 * @param life The player's new level of life.
 */
void Equipment::set_life(int life) {

  life = std::max(0, std::min(get_max_life(), life));
  savegame.set_integer(Savegame::KEY_CURRENT_LIFE, life);
}

/**
 * @brief Gives some life to the player.
 *
 * If the maximum life is reached, no more life is added.
 *
 * @param life_to_add level of life to add
 */
void Equipment::add_life(int life_to_add) {

  set_life(get_life() + life_to_add);
}

/**
 * @brief Removes some life from the player.
 * @param life_to_remove amount of life to remove
 */
void Equipment::remove_life(int life_to_remove) {

  set_life(get_life() - life_to_remove);
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
  return savegame.get_integer(Savegame::KEY_MAX_MAGIC);
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

  Debug::check_assertion(max_magic >= 0, StringConcat()
      << "Illegal maximum number of magic points: " << max_magic);

  savegame.set_integer(Savegame::KEY_MAX_MAGIC, max_magic);

  restore_all_magic();
}

/**
 * @brief Returns the current number of magic points of the player.
 * @return the player's current number of magic points
 */
int Equipment::get_magic() {
  return savegame.get_integer(Savegame::KEY_CURRENT_MAGIC);
}

/**
 * @brief Sets the current number of magic points of the player.
 *
 * If the value is lower than zero, it is replaced by zero.
 * If the value is greater than get_max_life(), it is replaced by that value.
 *
 * @param magic The player's new number of magic points.
 */
void Equipment::set_magic(int magic) {

  magic = std::max(0, std::min(get_max_magic(), magic));
  savegame.set_integer(Savegame::KEY_CURRENT_MAGIC, magic);
}

/**
 * @brief Adds some magic points to the player.
 *
 * If the maximum value is reached, no more magic points are added.
 * 
 * @param magic_to_add number of magic points to add
 */
void Equipment::add_magic(int magic_to_add) {

  set_magic(get_magic() + magic_to_add);
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

  set_magic(get_magic() - magic_to_remove);
}

/**
 * @brief Restores all magic points.
 */
void Equipment::restore_all_magic() {

  set_magic(get_max_magic());
}

// items

/**
 * @brief Runs the Lua script of each equipment item.
 */
void Equipment::load_items() {

  // TODO implement a separate class that provide the info of project_db.dat.

  // Load the list of equipment item names.
  static const std::string file_name = "project_db.dat";
  std::istream& database_file = FileTools::data_file_open(file_name);
  std::string line;

  while (std::getline(database_file, line)) {

    if (line.size() == 0) {
      continue;
    }

    int resource_type;
    std::string resource_id, resource_name;
    std::istringstream iss(line);
    FileTools::read(iss, resource_type);
    FileTools::read(iss, resource_id);
    FileTools::read(iss, resource_name);

    if (resource_type == 5) {  // It's an equipment item.
      EquipmentItem* item = new EquipmentItem(*this);
      item->increment_refcount();
      item->set_name(resource_id);
      items[resource_id] = item;
    }
  }
  FileTools::data_file_close(database_file);

  // Load the item scripts.
  std::map<std::string, EquipmentItem*>::const_iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    it->second->initialize();
  }

  // Start the items once they all exist.
  for (it = items.begin(); it != items.end(); it++) {
    it->second->start();
  }
}

/**
 * @brief Returns whether an equipment item with the specified name exists.
 * @param item_name Name of the item to check.
 * @return \c true if such an item exists.
 */
bool Equipment::item_exists(const std::string& item_name) {
  return items.find(item_name) != items.end();
}


/**
 * @brief Returns an equipment item.
 * @param item_name name of the item to get
 * @return the corresponding item
 */
EquipmentItem& Equipment::get_item(const std::string& item_name) {

  Debug::check_assertion(item_exists(item_name),
      StringConcat() << "Cannot find item with name '" << item_name << "'");

  return *items[item_name];
}

/**
 * @brief Returns the item currently assigned to a slot.
 * @param slot Slot of the item to get (1 or 2).
 * @return The item currently assigned to this slot or NULL.
 */
EquipmentItem* Equipment::get_item_assigned(int slot) {

  // TODO don't hardcode item slots

  Debug::check_assertion(slot >= 1 && slot <= 2, StringConcat() <<
      "Invalid item slot '" << slot << "'");

  std::ostringstream oss;
  oss << "_item_slot_" << slot;
  const std::string& item_name = savegame.get_string(oss.str());

  EquipmentItem* item = NULL;
  if (!item_name.empty()) {
    item = &get_item(item_name);
  }
  return item;
}

/**
 * @brief Assigns an item to a slot.
 *
 * The program exits with an error message if the specified item
 * cannot be assigned or if the player does not have it.
 * 
 * @param slot Slot to set (1 or 2).
 * @param item The item to assign to this slot or NULL to empty the slot.
 */
void Equipment::set_item_assigned(int slot, EquipmentItem* item) {

  Debug::check_assertion(slot >= 1 && slot <= 2, StringConcat() <<
      "Invalid item slot '" << slot << "'");

  std::ostringstream oss;
  oss << "_item_slot_" << slot;

  if (item != NULL) {
    Debug::check_assertion(item->get_variant() > 0, StringConcat()
        << "Cannot assign item '" << item->get_name() << "' because the player does not have it");
    Debug::check_assertion(item->is_assignable(), StringConcat()
        << "The item '" << item->get_name() << "' cannot be assigned");
    savegame.set_string(oss.str(), item->get_name());
  }
  else {
    savegame.set_string(oss.str(), "");
  }
}

/**
 * @brief Returns the slot (1 or 2) where the specified item is currently assigned.
 * @param item The item to find.
 * @return The slot of this item, or 0 if this item is not assigned
 */
int Equipment::get_item_slot(EquipmentItem& item) {

  for (int i = 1; i <= 2; ++i) {
    EquipmentItem* assigned_item = get_item_assigned(i);
    if (assigned_item != NULL && assigned_item->get_name() == item.get_name()) {
      return i;
    }
  }

  return 0;
}

// abilities

/**
 * @brief Returns the index of the savegame variable that stores the specified ability.
 * @param ability_name Name of the ability.
 * @return Name of the boolean savegame variable that stores this ability.
 */
const std::string Equipment::get_ability_savegame_variable(
    const std::string& ability_name) {

  std::string savegame_variable;

  if (ability_name == "tunic") {
    savegame_variable = Savegame::KEY_ABILITY_TUNIC;
  }
  else if (ability_name == "sword") {
    savegame_variable = Savegame::KEY_ABILITY_SWORD;
  }
  else if (ability_name == "sword_knowledge") {
    savegame_variable = Savegame::KEY_ABILITY_SWORD_KNOWLEDGE;
  }
  else if (ability_name == "shield") {
    savegame_variable = Savegame::KEY_ABILITY_SHIELD;
  }
  else if (ability_name == "lift") {
    savegame_variable = Savegame::KEY_ABILITY_LIFT;
  }
  else if (ability_name == "swim") {
    savegame_variable = Savegame::KEY_ABILITY_SWIM;
  }
  else if (ability_name == "run") {
    savegame_variable = Savegame::KEY_ABILITY_RUN;
  }
  else if (ability_name == "detect_weak_walls") {
    savegame_variable = Savegame::KEY_ABILITY_DETECT_WEAK_WALLS;
  }
  else if (ability_name == "get_back_from_death") {
    savegame_variable = Savegame::KEY_ABILITY_GET_BACK_FROM_DEATH;
  }
  else {
    Debug::die(StringConcat() << "Unknown ability '" << ability_name << "'");
  }

  return savegame_variable;
}

/**
 * @brief Returns whether the player has at least the specified level of an ability.
 * @param ability_name the ability to get
 * @param level the minimum level to check
 * @return true if the player has at least this level of the ability
 */
bool Equipment::has_ability(const std::string& ability_name, int level) {
  return get_ability(ability_name) >= level;
}

/**
 * @brief Returns the level of the specified ability.
 * @param ability_name the ability to get
 * @return the level of this ability
 */
int Equipment::get_ability(const std::string& ability_name) {
  return savegame.get_integer(get_ability_savegame_variable(ability_name));
}

/**
 * @brief Sets the level of the specified ability.
 * @param ability_name the ability to set
 * @param level the level of this ability
 */
void Equipment::set_ability(const std::string& ability_name, int level) {

  savegame.set_integer(get_ability_savegame_variable(ability_name), level);

  Game* game = get_game();
  if (game != NULL) {
    if (ability_name == "tunic" ||
        ability_name == "sword" ||
        ability_name == "shield") {
      // The hero's sprites depend on these abilities.
      game->get_hero().rebuild_equipment();
    }
  }
}

/**
 * @brief This function is called when the player has just used an ability.
 *
 * All item scripts are notified.
 *
 * @param ability_name the ability used
 */
void Equipment::notify_ability_used(const std::string& ability_name) {

  std::map<std::string, EquipmentItem*>::iterator it;
  for (it = items.begin(); it != items.end(); it++) {
    it->second->notify_ability_used(ability_name);
  }
}

