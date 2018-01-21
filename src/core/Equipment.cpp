/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/Debug.h"
#include "solarus/core/Equipment.h"
#include "solarus/core/EquipmentItem.h"
#include "solarus/core/Game.h"
#include "solarus/core/Map.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Random.h"
#include "solarus/core/Savegame.h"
#include "solarus/core/System.h"
#include "solarus/entities/Hero.h"
#include <algorithm>
#include <sstream>

namespace Solarus {

/**
 * \brief Constructor.
 * \param savegame The savegame to encapsulate.
 */
Equipment::Equipment(Savegame& savegame):
  savegame(savegame),
  suspended(true) {

}

/**
 * \brief Returns the savegame represented by this equipment object.
 * \return The savegame.
 */
Savegame& Equipment::get_savegame() {
  return savegame;
}

/**
 * \brief If this equipment object is currently running in a game, return that game.
 * \return A game or nullptr.
 */
Game* Equipment::get_game() {
  return savegame.get_game();
}

/**
 * \brief Notifies the equipment that the game has just started.
 */
void Equipment::notify_game_started() {
}

/**
 * \brief Notifies the equipment that the game has is finished.
 */
void Equipment::notify_game_finished() {

  // The equipment items will disappear: notify them.
  for (const auto& kvp: items) {
    EquipmentItem& item = *kvp.second;
    item.exit();
  }
}

/**
 * \brief Notifies the equipment system has another map has just been started.
 * \param map the new current map
 */
void Equipment::notify_map_changed(Map& map) {

  // Notify the items.
  for (const auto& kvp: items) {
    EquipmentItem& item = *kvp.second;
    item.notify_map_changed(map);
  }
}

/**
 * \brief This function is be called repeatedly by the game.
 *
 * Most of the time, there is nothing to update in this class.
 * The only elements updated here are some dynamic things
 * such as equipment-related animations or dialogs.
 */
void Equipment::update() {

  Game* game = savegame.get_game();
  if (game == nullptr) {
    // Nothing dynamic when there is no game.
    return;
  }

  // Check if the game is suspended.
  bool game_suspended = game->is_suspended();
  if (suspended != game_suspended) {
    set_suspended(game_suspended);
  }

  // Update item scripts.
  for (const auto& kvp: items) {
    EquipmentItem& item = *kvp.second;
    item.update();
  }
}

/**
 * \brief This function is called when the game is suspended or resumed.
 * \param suspended true if the game is suspended, false if it is resumed
 */
void Equipment::set_suspended(bool suspended) {

  this->suspended = suspended;

  // Notify the item scripts.
  for (const auto& kvp: items) {
    EquipmentItem& item = *kvp.second;
    item.set_suspended(suspended);
  }
}

/**
 * \brief Returns the maximum amount of money of the player.
 * \return the player's maximum number of money
 */
int Equipment::get_max_money() const {
  return savegame.get_integer(Savegame::KEY_MAX_MONEY);
}

/**
 * \brief Sets the maximum amount of money of the player.
 * \param max_money the player's maximum amount number of money
 */
void Equipment::set_max_money(int max_money) {

  Debug::check_assertion(max_money >= 0, "Invalid money amount to add");

  savegame.set_integer(Savegame::KEY_MAX_MONEY, max_money);

  // If the max money is reduced, make sure the current money does not exceed
  // the new maximum.
  if (get_money() > get_max_money()) {
    set_money(max_money);
  }
}

/**
 * \brief Returns the player's current amount of money
 * \return the player's current amount of money
 */
int Equipment::get_money() const {
  return savegame.get_integer(Savegame::KEY_CURRENT_MONEY);
}

/**
 * \brief Sets the player's current amount of money.
 *
 * If the amount is lower than zero, it is replaced by zero.
 * If the amount is greater than get_max_money(), it is replaced by that value.
 *
 * \param money The player's new amount of money.
 */
void Equipment::set_money(int money) {

  money = std::max(0, std::min(get_max_money(), money));
  savegame.set_integer(Savegame::KEY_CURRENT_MONEY, money);
}

/**
 * \brief Adds some money to the player.
 *
 * If the maximum amount of money is reached, no more money is added.
 *
 * \param money_to_add Amount of money to add.
 * Must be positive of zero.
 */
void Equipment::add_money(int money_to_add) {

  Debug::check_assertion(money_to_add >= 0, "Invalid money amount to add");

  set_money(get_money() + money_to_add);
}

/**
 * \brief Removes some money from the player.
 *
 * If the money reaches zero, no more money is removed.
 *
 * \param money_to_remove Amount of money to remove.
 * Must be positive of zero.
 */
void Equipment::remove_money(int money_to_remove) {

  Debug::check_assertion(money_to_remove >= 0, "Invalid money amount to remove");

  set_money(get_money() - money_to_remove);
}

/**
 * \brief Returns the maximum level of life of the player.
 * \return the player's maximum level of life
 */
int Equipment::get_max_life() const {
  return savegame.get_integer(Savegame::KEY_MAX_LIFE);
}

/**
 * \brief Sets the maximum level of life of the player.
 *
 * The program exits with an error message if the specified maximum
 * life is not valid.
 *
 * \param max_life the player's maximum life
 */
void Equipment::set_max_life(int max_life) {

  Debug::check_assertion(max_life >= 0, "Invalid life amount");

  savegame.set_integer(Savegame::KEY_MAX_LIFE, max_life);

  // If the max life is reduced, make sure the current life does not exceed
  // the new maximum.
  if (get_life() > get_max_life()) {
    set_life(max_life);
  }
}

/**
 * \brief Returns the current level of life of the player.
 * \return the player's current life
 */
int Equipment::get_life() const {
  return savegame.get_integer(Savegame::KEY_CURRENT_LIFE);
}

/**
 * \brief Sets the current life of the player.
 *
 * If the life is lower than zero, it is replaced by zero.
 * If the life is greater than get_max_life(), it is replaced by that value.
 *
 * \param life The player's new level of life.
 */
void Equipment::set_life(int life) {

  life = std::max(0, std::min(get_max_life(), life));
  savegame.set_integer(Savegame::KEY_CURRENT_LIFE, life);
}

/**
 * \brief Gives some life to the player.
 *
 * If the maximum life is reached, no more life is added.
 *
 * \param life_to_add Level of life to add.
 * Must be positive of zero.
 */
void Equipment::add_life(int life_to_add) {

  Debug::check_assertion(life_to_add >= 0, "Invalid life amount to add");

  set_life(get_life() + life_to_add);
}

/**
 * \brief Removes some life from the player.
 * \param life_to_remove Amount of life to remove.
 * Must be positive of zero.
 */
void Equipment::remove_life(int life_to_remove) {

  Debug::check_assertion(life_to_remove >= 0, "Invalid life amount to remove");

  set_life(get_life() - life_to_remove);
}

/**
 * \brief Restores all the life.
 */
void Equipment::restore_all_life() {

  set_life(get_max_life());
}

/**
 * \brief Returns the maximum number of magic points.
 * \return the maximum level of magic
 */
int Equipment::get_max_magic() const {
  return savegame.get_integer(Savegame::KEY_MAX_MAGIC);
}

/**
 * \brief Sets the maximum number of magic points.
 *
 * Exits with an error message if the value specified
 * if not valid.
 *
 * \param max_magic the maximum level of magic
 */
void Equipment::set_max_magic(int max_magic) {

  Debug::check_assertion(max_magic >= 0, "Invalid magic amount");

  savegame.set_integer(Savegame::KEY_MAX_MAGIC, max_magic);

  restore_all_magic();
}

/**
 * \brief Returns the current number of magic points of the player.
 * \return the player's current number of magic points
 */
int Equipment::get_magic() const {
  return savegame.get_integer(Savegame::KEY_CURRENT_MAGIC);
}

/**
 * \brief Sets the current number of magic points of the player.
 *
 * If the value is lower than zero, it is replaced by zero.
 * If the value is greater than get_max_life(), it is replaced by that value.
 *
 * \param magic The player's new number of magic points.
 * Must be positive of zero.
 */
void Equipment::set_magic(int magic) {

  magic = std::max(0, std::min(get_max_magic(), magic));
  savegame.set_integer(Savegame::KEY_CURRENT_MAGIC, magic);
}

/**
 * \brief Adds some magic points to the player.
 *
 * If the maximum value is reached, no more magic points are added.
 *
 * \param magic_to_add Number of magic points to add.
 * Must be positive of zero.
 */
void Equipment::add_magic(int magic_to_add) {

  Debug::check_assertion(magic_to_add >= 0, "Invalid magic amount to add");

  set_magic(get_magic() + magic_to_add);
}

/**
 * \brief Removes some magic points from the player.
 *
 * If the number of magic points reaches zero, no more magic points
 * are removed.
 *
 * \param magic_to_remove Number of magic poits to remove.
 * Must be positive of zero.
 */
void Equipment::remove_magic(int magic_to_remove) {

  Debug::check_assertion(magic_to_remove >= 0, "Invalid magic amount to remove");

  set_magic(get_magic() - magic_to_remove);
}

/**
 * \brief Restores all magic points.
 */
void Equipment::restore_all_magic() {

  set_magic(get_max_magic());
}

/**
 * \brief Runs the Lua script of each equipment item.
 */
void Equipment::load_items() {

  // Create each equipment item declared in project_db.dat.
  const std::map<std::string, std::string>& item_elements =
      CurrentQuest::get_resources(ResourceType::ITEM);
  for (const auto& kvp: item_elements) {
    const std::string& item_id = kvp.first;
    std::shared_ptr<EquipmentItem> item = std::make_shared<EquipmentItem>(*this);
    item->set_name(item_id);
    items[item_id] = item;
  }

  // Load the item scripts.
  for (const auto& kvp: items) {
    EquipmentItem& item = *kvp.second;
    item.initialize();
  }

  // Start the items once they all exist.
  for (const auto& kvp: items) {
    EquipmentItem& item = *kvp.second;
    item.start();
  }
}

/**
 * \brief Returns whether an equipment item with the specified name exists.
 * \param item_name Name of the item to check.
 * \return \c true if such an item exists.
 */
bool Equipment::item_exists(const std::string& item_name) const {
  return items.find(item_name) != items.end();
}


/**
 * \brief Returns an equipment item.
 * \param item_name Name of the item to get.
 * \return The corresponding item.
 */
EquipmentItem& Equipment::get_item(const std::string& item_name) {

  Debug::check_assertion(item_exists(item_name),
      std::string("No such item: '") + item_name + "'");

  return *items.find(item_name)->second;
}

/**
 * \brief Returns an equipment item.
 * \param item_name Name of the item to get.
 * \return The corresponding item.
 */
const EquipmentItem& Equipment::get_item(const std::string& item_name) const {

  Debug::check_assertion(item_exists(item_name),
      std::string("No such item: '") + item_name + "'");

  return *items.find(item_name)->second;
}

/**
 * \brief Returns the item currently assigned to a slot.
 * \param slot Slot of the item to get (1 or 2).
 * \return The item currently assigned to this slot or nullptr.
 */
EquipmentItem* Equipment::get_item_assigned(int slot) {

  // TODO don't hardcode item slots

  Debug::check_assertion(slot >= 1 && slot <= 2,
      "Invalid item slot");

  char savegame_variable[] = "_item_slot_0";
  savegame_variable[11] = '0' + slot;
  const std::string& item_name = savegame.get_string(savegame_variable);

  EquipmentItem* item = nullptr;
  if (!item_name.empty()) {
    item = &get_item(item_name);
  }
  return item;
}

/**
 * \brief Returns the item currently assigned to a slot.
 * \param slot Slot of the item to get (1 or 2).
 * \return The item currently assigned to this slot or nullptr.
 */
const EquipmentItem* Equipment::get_item_assigned(int slot) const {

  Debug::check_assertion(slot >= 1 && slot <= 2,
      "Invalid item slot");

  std::ostringstream oss;
  oss << "_item_slot_" << slot;
  const std::string& item_name = savegame.get_string(oss.str());

  const EquipmentItem* item = nullptr;
  if (!item_name.empty()) {
    item = &get_item(item_name);
  }
  return item;
}

/**
 * \brief Assigns an item to a slot.
 *
 * The program exits with an error message if the specified item
 * cannot be assigned or if the player does not have it.
 *
 * \param slot Slot to set (1 or 2).
 * \param item The item to assign to this slot or nullptr to empty the slot.
 */
void Equipment::set_item_assigned(int slot, EquipmentItem* item) {

  Debug::check_assertion(slot >= 1 && slot <= 2,
      "Invalid item slot");

  std::ostringstream oss;
  oss << "_item_slot_" << slot;

  if (item != nullptr) {
    Debug::check_assertion(item->get_variant() > 0,
        std::string("Cannot assign item '") + item->get_name()
        + "' because the player does not have it");
    Debug::check_assertion(item->is_assignable(),
        std::string("The item '") + item->get_name()
        + "' cannot be assigned");
    savegame.set_string(oss.str(), item->get_name());
  }
  else {
    savegame.set_string(oss.str(), "");
  }
}

/**
 * \brief Returns the slot (1 or 2) where the specified item is currently assigned.
 * \param item The item to find.
 * \return The slot of this item, or 0 if this item is not assigned
 */
int Equipment::get_item_slot(const EquipmentItem& item) const {

  for (int i = 1; i <= 2; ++i) {
    const EquipmentItem* assigned_item = get_item_assigned(i);
    if (assigned_item != nullptr && assigned_item->get_name() == item.get_name()) {
      return i;
    }
  }

  return 0;
}

// Abilities.

/**
 * \brief Returns the savegame variable that stores the specified ability.
 * \param ability An ability.
 * \return Name of the integer savegame variable that stores this ability.
 */
std::string Equipment::get_ability_savegame_variable(Ability ability) const {

  switch (ability) {

  case Ability::TUNIC:
    return Savegame::KEY_ABILITY_TUNIC;

  case Ability::SWORD:
    return Savegame::KEY_ABILITY_SWORD;

  case Ability::SWORD_KNOWLEDGE:
    return Savegame::KEY_ABILITY_SWORD_KNOWLEDGE;

  case Ability::SHIELD:
    return Savegame::KEY_ABILITY_SHIELD;

  case Ability::LIFT:
    return Savegame::KEY_ABILITY_LIFT;

  case Ability::SWIM:
    return Savegame::KEY_ABILITY_SWIM;

  case Ability::JUMP_OVER_WATER:
    return Savegame::KEY_ABILITY_JUMP_OVER_WATER;

  case Ability::RUN:
    return Savegame::KEY_ABILITY_RUN;

  case Ability::PUSH:
    return Savegame::KEY_ABILITY_PUSH;

  case Ability::GRAB:
    return Savegame::KEY_ABILITY_GRAB;

  case Ability::PULL:
    return Savegame::KEY_ABILITY_PULL;

  case Ability::DETECT_WEAK_WALLS:
    return Savegame::KEY_ABILITY_DETECT_WEAK_WALLS;
  }

  Debug::die("Invalid ability");
}

/**
 * \brief Returns whether the player has at least the specified level of an ability.
 * \param ability The ability to get.
 * \param level The minimum level to check.
 * \return \c true if the player has at least this level of the ability.
 */
bool Equipment::has_ability(Ability ability, int level) const {
  return get_ability(ability) >= level;
}

/**
 * \brief Returns the level of the specified ability.
 * \param ability The ability to get.
 * \return The level of this ability.
 */
int Equipment::get_ability(Ability ability) const {
  return savegame.get_integer(get_ability_savegame_variable(ability));
}

/**
 * \brief Sets the level of the specified ability.
 * \param ability The ability to set.
 * \param level The level of this ability.
 */
void Equipment::set_ability(Ability ability, int level) {

  savegame.set_integer(get_ability_savegame_variable(ability), level);

  Game* game = get_game();
  if (game != nullptr) {
    if (ability == Ability::TUNIC ||
        ability == Ability::SWORD ||
        ability == Ability::SHIELD) {
      // The hero's sprites may depend on these abilities.
      game->get_hero()->rebuild_equipment();
    }
  }
}

/**
 * \brief This function is called when the player has just used an ability.
 *
 * All item scripts are notified.
 *
 * \param ability The ability used.
 */
void Equipment::notify_ability_used(Ability ability) {

  for (const auto& kvp: items) {
    EquipmentItem& item = *kvp.second;
    item.notify_ability_used(ability);
  }
}

}

