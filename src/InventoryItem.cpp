/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "InventoryItem.h"
#include "Savegame.h"

/**
 * Properties of each item of the inventory.
 */
InventoryItem InventoryItem::items[28] = {

  InventoryItem(FEATHER, true, 0),
  InventoryItem(BOMBS, true, Savegame::CURRENT_BOMBS),
  InventoryItem(BOW, true, Savegame::CURRENT_ARROWS),
  InventoryItem(BOOMERANG, true, 0),
  InventoryItem(LAMP, true, 0),
  InventoryItem(HOOK_SHOT, true, 0),
  InventoryItem(BOTTLE_1, true, 0),

  InventoryItem(PEGASUS_SHOES, true, 0),
  InventoryItem(MYSTIC_MIRROR, true, 0),
  InventoryItem(CANE_OF_SOMARIA, true, 0),
  InventoryItem(APPLES, true, Savegame::CURRENT_APPLES),
  InventoryItem(PAINS_AU_CHOCOLAT, true, Savegame::CURRENT_PAINS_AU_CHOCOLAT),
  InventoryItem(CROISSANTS, true, Savegame::CURRENT_CROISSANTS),
  InventoryItem(BOTTLE_2, true, 0),

  InventoryItem(ROCK_KEY, false, 0),
  InventoryItem(RED_KEY, false, 0),
  InventoryItem(CLAY_KEY, false, 0),
  InventoryItem(L4_WAY_BONE_KEY, false, 0),
  InventoryItem(FLIPPERS, false, 0),
  InventoryItem(MAGIC_CAPE, false, 0),
  InventoryItem(BOTTLE_3, true, 0),


  InventoryItem(IRON_KEY, false, 0),
  InventoryItem(STONE_KEY, false, 0),
  InventoryItem(WOODEN_KEY, false, 0),
  InventoryItem(ICE_KEY, false, 0),
  InventoryItem(GLOVE, false, 0),
  InventoryItem(FIRE_STONES, false, Savegame::NB_FIRE_STONES),
  InventoryItem(BOTTLE_4, true, 0),
};

/**
 * Constructor.
 * @param id id of this item
 * @param attributable true if this item can be assigned to icon X or V
 * @param counter_index index of the savegame variable indicating the
 * counter's value (0 for no counter)
 */
InventoryItem::InventoryItem(ItemId id, bool attributable, int counter_index):
  id(id), attributable(attributable), counter_index(counter_index) {

}

/**
 * Destructor.
 */
InventoryItem::~InventoryItem(void) {

}

/**
 * Returns an inventory item specified by its id.
 * @param id id of the inventory item to get
 */
InventoryItem * InventoryItem::get_item(ItemId id) {
  return &items[id];
}

/**
 * Returns whether the current item can be assigned to icon X or V.
 * @return true if the current item is attributable
 */
bool InventoryItem::is_attributable(void) {
  return attributable;
}

/**
 * Returns whether a counter is associated to this item.
 * This is equivalent to get_counter_index() != 0.
 */
bool InventoryItem::has_counter(void) {
  return counter_index != 0;
}

/**
 * If this item has a counter, returns the index of the savegame
 * variable indicating the counter's value. Otherwise, returns 0.
 * @return the index of the savegame variable indicating the counter's value
 */
int InventoryItem::get_counter_index(void) {
  return counter_index;
}

/**
 * Uses this item.
 */
void InventoryItem::use(void) {
  
}

/**
 * Updates this item when it is being used.
 */
void InventoryItem::update(void) {

}

/**
 * Returns whether this item has finished to be used.
 * @return true if this item has finished to be used
 */
bool InventoryItem::is_finished(void) {
  return true;
}
