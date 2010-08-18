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
 * @param item_id id of the inventory item to use
 */
Hero::InventoryItemState::InventoryItemState(Hero *hero, InventoryItemId item_id):
  State(hero), item(new InventoryItem(item_id)) {

}

/**
 * @brief Destructor.
 */
Hero::InventoryItemState::~InventoryItemState(void) {
  delete item;
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::InventoryItemState::start(State *previous_state) {

  State::start(previous_state);

  hero->can_use_inventory_item_date = System::now() + 500;
  item->start(game);
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::InventoryItemState::stop(State *next_state) {

  delete item;
  item = NULL;
}

/**
 * @brief Updates this state.
 */
void Hero::InventoryItemState::update(void) {

  State::update();

  item->update();
  if (item->is_finished() && is_current_state()) {
    // if the state was not modified by the item, return to the normal state
    hero->set_state(new FreeState(hero));
  }
}

