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
#ifndef SOLARUS_INVENTORY_ITEM_H
#define SOLARUS_INVENTORY_ITEM_H

#include "Common.h"
#include "Savegame.h"
#include "InventoryItemId.h"

/**
 * @brief Represents an item from the inventory.
 *
 * This class provides the description of each item of the inventory
 * with some static methods,
 * and handles their behavior when it is instanciated.
 * This inventory items does not include the dungeon items (map, compass, etc.)
 * nor the items of the quest status screen (rupee bag, heart pieces, etc.).
 * It only includes the items displayed on the inventory submenu
 * (accessible when the game is paused).
 * Some inventory items can be assigned to the two inventory item keys
 * and then be used directly during the game (e.g. the boomerang, the bombs, etc.),
 * whereas some other cannot (e.g. the glove and the flippers)
 * because they are only used in an implicit way.
 */
class InventoryItem {

  private:

    InventoryItemId item_id;	/**< id of this item */

    // state
    int variant;		/**< the possession state of this item when it is used */
    Game *game;			/**< the game this item is used in */
    bool finished;		/**< true if we have finished using this item */

    // additionnal state information used by certain items
    SoundId item_sound_id;	/**< sound played repeatedly while using this item */
    uint32_t sound_delay;	/**< delay before playing the sound again */
    uint32_t next_sound_date;	/**< when the sound is played again */
    int direction_pressed8;	/**< direction defined by the directional keys pressed when starting using this item
				 * (0 to 8) or -1 if there is no direction */

    void start_bottle(void);
    void update_bottle(void);

  public:

    // creation and destruction
    InventoryItem(InventoryItemId item_id);
    ~InventoryItem(void);

    // static features
    static bool can_be_assigned(InventoryItemId item_id);
    static bool has_counter(InventoryItemId item_id);
    static int get_counter_index(InventoryItemId item_id);
    static bool is_bottle(InventoryItemId item_id);

    // properties
    InventoryItemId get_id(void);
    int get_variant(void);
    bool is_bottle(void);

    // state
    void start(Game *game);
    void update(void);
    void set_map(Map *map);
    bool is_finished(void);
};

#endif

