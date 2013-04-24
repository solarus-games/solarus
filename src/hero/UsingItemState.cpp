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
#include "hero/UsingItemState.h"
#include "hero/FreeState.h"
#include "lowlevel/System.h"
#include "entities/Detector.h"

/**
 * @brief Constructor.
 * @param hero The hero controlled by this state.
 * @param item The equipment item to use.
 */
Hero::UsingItemState::UsingItemState(Hero& hero,
    EquipmentItem& item):
  State(hero),
  item_usage(hero.get_game(), item) {

}

/**
 * @brief Destructor.
 */
Hero::UsingItemState::~UsingItemState() {
}

/**
 * @brief Starts this state.
 * @param previous_state The previous state.
 */
void Hero::UsingItemState::start(State* previous_state) {

  State::start(previous_state);

  bool interaction = false;
  Detector* facing_entity = hero.get_facing_entity();
  if (facing_entity != NULL && !facing_entity->is_being_removed()) {
    // Maybe the facing entity (e.g. an NPC) accepts an interaction with this
    // particular item.
    interaction = facing_entity->interaction_with_item(item_usage.get_item());
  }

  if (!interaction) {
    // no interaction occurred with the facing entity: use the item normally
    item_usage.start();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::UsingItemState::update() {

  State::update();

  item_usage.update();
  if (item_usage.is_finished() && is_current_state()) {
    // if the state was not modified by the item, return to the normal state
    hero.set_state(new FreeState(hero));
  }
}

/**
 * @brief Returns whether the hero is using an equipment item in this state.
 * @return true if the hero is using an equipment item.
 */
bool Hero::UsingItemState::is_using_item() {
  return true;
}

/**
 * @brief When the hero is using an equipment item, returns that item.
 * @return The current equipment item.
 */
EquipmentItemUsage& Hero::UsingItemState::get_item_being_used() {
  return item_usage;
}
