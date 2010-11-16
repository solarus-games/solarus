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
#include "lua/ItemScript.h"
#include "ItemProperties.h"
#include "InventoryItem.h"
#include "Game.h"
#include "Treasure.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

/**
 * @brief Constructor.
 * @param game the game
 * @param item_properties the static properties of the item
 */
ItemScript::ItemScript(Game &game, ItemProperties &item_properties):
  Script(MAIN_API | GAME_API | MAP_API | ITEM_API),
  game(game),
  item_properties(item_properties) {

  std::string script_name = (std::string) "items/" + item_properties.get_name();
  load_if_exists(script_name);
}

/**
 * @brief Destructor.
 */
ItemScript::~ItemScript() {

}

/**
 * @brief Returns the game that runs this script.
 * @return the game
 */
Game& ItemScript::get_game() {
  return game;
}

/**
 * @brief Returns the current map of the game.
 * @return the map
 */
Map& ItemScript::get_map() {
  return game.get_current_map();
}

/**
 * @brief Returns the properties of the equipment item controlled by this script.
 * @return the item properties
 */
ItemProperties& ItemScript::get_item_properties() {
  return item_properties;
}

/**
 * @brief Returns the pickable item related to the current call to event_appear().
 * @return the current pickable item, or NULL
 */
PickableItem* ItemScript::get_pickable_item() {
  return pickable_item;
}

/**
 * @brief Returns the inventory item related to the current call to event_use().
 * @return the current inventory item, or NULL
 */
InventoryItem* ItemScript::get_inventory_item() {
  return inventory_item;
}

/**
 * @brief Notifies the script that a pickable item of its type has just appeared on the map.
 * @param pickable_item the pickable item
 */
void ItemScript::event_appear(PickableItem &pickable_item) {

  this->pickable_item = &pickable_item;
  notify_script("event_appear");
  this->pickable_item = NULL;
}

/**
 * @brief Notifies the script that the player is obtaining a treasure of its type.
 * @param treasure the treasure
 */
void ItemScript::event_obtaining(const Treasure &treasure) {

  notify_script("event_obtaining", "ii", treasure.get_variant(), treasure.get_savegame_variable());
}

/**
 * @brief Notifies the script that the player has just obtained a treasure of its type.
 * @param treasure the treasure
 */
void ItemScript::event_obtained(const Treasure &treasure) {

  notify_script("event_obtained", "ii", treasure.get_variant(), treasure.get_savegame_variable());
}

/**
 * @brief Notifies the script that the amount of this item has just changed.
 * @param amount the new amount
 */
void ItemScript::event_amount_changed(int amount) {

  notify_script("event_amount_changed", "i", amount);
}

/**
 * @brief Notifies the script that the player starts using an inventory item of its type.
 * @param inventory_item the inventory item
 */
void ItemScript::event_use(InventoryItem &inventory_item) {

  this->inventory_item = &inventory_item;
  if (!notify_script("event_use")) {
    Debug::die(StringConcat() << "No script for inventory item '" << item_properties.get_name());
  }
  this->inventory_item = NULL;
}

