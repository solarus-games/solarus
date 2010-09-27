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
#include "hero/InventoryItemState.h"
#include "hero/FreeState.h"
#include "lowlevel/System.h"
#include "InventoryItem.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param item_name name of the inventory item to use
 */
Hero::InventoryItemState::InventoryItemState(Hero *hero, const std::string &item_name):
  State(hero), item(new InventoryItem(item_name)) {

}

/**
 * @brief Destructor.
 */
Hero::InventoryItemState::~InventoryItemState() {
  delete item;
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::InventoryItemState::start(State *previous_state) {

  State::start(previous_state);

  hero->last_inventory_item_name = item->get_name();
  // TODO delay of 500 ms for the bow
  // hero->can_use_inventory_item_date = System::now() + item->get_reuse_delay();
  hero->can_use_inventory_item_date = System::now();
  item->start(*game);
}

/**
 * @brief Updates this state.
 */
void Hero::InventoryItemState::update() {

  State::update();

  item->update();
  if (item->is_finished() && is_current_state()) {
    // if the state was not modified by the item, return to the normal state
    hero->set_state(new FreeState(hero));
  }
}

