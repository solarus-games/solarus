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
#include "Game.h"
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

  std::string script_name = (std::string) "items" + get_item_name();
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
 * @brief Returns the name of the equipment item controlled by this script.
 * @return the item name
 */
const std::string& ItemScript::get_item_name() {
  return item_properties.get_name();
}

// TODO
void ItemScript::event_appear(PickableItem &pickable_item) {

}

void ItemScript::event_obtaining(const Treasure &treasure) {

}

void ItemScript::event_obtained(const Treasure &treasure) {

}

void ItemScript::event_use(InventoryItem &inventory_item) {

}

