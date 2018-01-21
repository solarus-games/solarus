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
#ifndef SOLARUS_INVENTORY_ITEM_H
#define SOLARUS_INVENTORY_ITEM_H

#include "solarus/core/Common.h"
#include "solarus/core/Savegame.h"

namespace Solarus {

/**
 * \brief Represents an equipment item being used by the hero.
 *
 * This class handles the behavior of the item when the player uses it.
 */
class EquipmentItemUsage {

  public:

    // creation and destruction
    EquipmentItemUsage(Game& game, EquipmentItem& item);

    // item information
    EquipmentItem& get_item();

    // state
    void start();
    void update();
    bool is_finished();
    void set_finished();

  private:

    EquipmentItem& item;     /**< name of the item being used */
    int variant;             /**< the possession state of this item when it is used */

    // state
    bool finished;           /**< true if we have finished using this item or not using it */

};

}

#endif

