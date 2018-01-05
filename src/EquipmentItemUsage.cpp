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
#include "solarus/EquipmentItemUsage.h"
#include "solarus/Game.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItem.h"
#include "solarus/lowlevel/Debug.h"
#include <string>

namespace Solarus {

/**
 * \brief Creates a new item to be used.
 * \param game The game.
 * \param item The item to use.
 */
EquipmentItemUsage::EquipmentItemUsage(Game& /* game */, EquipmentItem& item):
  item(item),
  variant(item.get_variant()),
  finished(true) {

}

/**
 * \brief Returns the equipment item being used.
 */
EquipmentItem& EquipmentItemUsage::get_item() {
  return item;
}

/**
 * \brief Starts using this item.
 */
void EquipmentItemUsage::start() {

  Debug::check_assertion(variant > 0,
      std::string("Attempt to use equipment item '") + item.get_name()
      + "' without having it");

  this->finished = false;
  item.notify_using();
}

/**
 * \brief Updates this item when it is being used.
 */
void EquipmentItemUsage::update() {
}

/**
 * \brief Returns whether this item has finished to be used.
 * \return true if this item has finished to be used
 */
bool EquipmentItemUsage::is_finished() {
  return finished;
}

/**
 * \brief Indicates that the player has finished using this item.
 */
void EquipmentItemUsage::set_finished() {
  this->finished = true;
}

}

