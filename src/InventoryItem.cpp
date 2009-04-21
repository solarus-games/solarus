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
 * Constructor.
 * @param item_id id of the item to create
 */
InventoryItem::InventoryItem(InventoryItemId item_id):
  item_id(item_id) {

}

/**
 * Destructor.
 */
InventoryItem::~InventoryItem(void) {

}

/**
 * Returns whether the specified item can be assigned to icon X or V.
 * @param item_id id of a item
 * @return true if this item item is attributable
 */
bool InventoryItem::is_attributable(InventoryItemId item_id) {
  return item_id < ITEM_ROCK_KEY;
}

/**
 * Returns whether a counter is associated to the specified item.
 * This is equivalent to get_counter_index(item_id) != 0.
 * @param item_id id of an item
 * @return true if this item has a counter
 */
bool InventoryItem::has_counter(InventoryItemId item_id) {
  return get_counter_index(item_id) != -1;
}

/**
 * If the specified item has a counter, returns the index of the savegame
 * variable indicating the counter's value. Otherwise, returns -1.
 * @return the index of the savegame variable indicating the counter's value
 */
int InventoryItem::get_counter_index(InventoryItemId item_id) {

  int counter;

  switch(item_id) {

  case ITEM_BOMBS:
    counter = Savegame::CURRENT_BOMBS;
    break;

  case ITEM_BOW:
    counter = Savegame::CURRENT_ARROWS;
    break;

  case ITEM_APPLES:
    counter = Savegame::CURRENT_APPLES;
    break;

  case ITEM_PAINS_AU_CHOCOLAT:
    counter = Savegame::CURRENT_PAINS_AU_CHOCOLAT;
    break;

  case ITEM_CROISSANTS:
    counter = Savegame::CURRENT_CROISSANTS;
    break;

  case ITEM_FIRE_STONES:
    counter = Savegame::NB_FIRE_STONES;
    break;

  default:
    counter = -1;
    break;
  }

  return counter;
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
