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
#ifndef SOLARUS_ITEM_SCRIPT_H
#define SOLARUS_ITEM_SCRIPT_H

#include "Common.h"
#include "lua/MapScript.h"

/**
 * @brief Represents the Lua script of an item of the equipment.
 *
 * Such a script only exist in-game since it manages the dynamic behavior of an item.
 * The static properties of an item are handled by the ItemProperties class.
 * If the Lua file of the item does not exist, this class has no effect.
 *
 * This class makes the interface between the engine C++ code and the Lua script of an item.
 */
class ItemScript: public Script {

  private:

    Game &game;						/**< the game */
    ItemProperties &item_properties;			/**< static properties of the item */

    PickableItem *pickable_item;			/**< the pickable item that just appeared in event_appear(), or NULL */
    InventoryItem *inventory_item;			/**< the inventory item that is being used when event_used() is called,
    							 * or NULL if no inventory item is being used */

  protected:

    Game &get_game();
    Map &get_map();
    ItemProperties& get_item_properties();

  public:

    ItemScript(Game &game, ItemProperties &item_properties);
    ~ItemScript();

    void set_suspended(bool suspended);

    void event_set_suspended(bool suspended);
    void event_appear(PickableItem &pickable_item);
    void event_obtaining(const Treasure &treasure);
    void event_obtained(const Treasure &treasure);
    void event_amount_changed(int amount);
    void event_use(InventoryItem &inventory_item);

    PickableItem* get_pickable_item();
    InventoryItem* get_inventory_item();
};

#endif

