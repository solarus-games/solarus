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
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "DialogBox.h"
#include "MapScript.h"
#include "entities/Hero.h"
#include "entities/Detector.h"

/**
 * Creates a new inventory item.
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
 * @return true if this item item can be assigned to an icon
 */
bool InventoryItem::can_be_assigned(InventoryItemId item_id) {
  return item_id < INVENTORY_ROCK_KEY;
}

/**
 * Returns whether a counter is associated to the specified item.
 * This is equivalent to get_counter_index(item_id) != -1.
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

  case INVENTORY_BOMBS:
    counter = Savegame::CURRENT_BOMBS;
    break;

  case INVENTORY_BOW:
    counter = Savegame::CURRENT_ARROWS;
    break;

  case INVENTORY_APPLES:
    counter = Savegame::CURRENT_APPLES;
    break;

  case INVENTORY_PAINS_AU_CHOCOLAT:
    counter = Savegame::CURRENT_PAINS_AU_CHOCOLAT;
    break;

  case INVENTORY_CROISSANTS:
    counter = Savegame::CURRENT_CROISSANTS;
    break;

  case INVENTORY_FIRE_STONES:
    counter = Savegame::NB_FIRE_STONES;
    break;

  default:
    counter = -1;
    break;
  }

  return counter;
}

/**
 * Returns the id of this inventory item.
 * @return the id of this inventory item
 */
InventoryItemId InventoryItem::get_id(void) {
  return item_id;
}

/**
 * Returns the variant of this inventory item that the player has.
 * @return the variant
 */
int InventoryItem::get_variant(void) {
  return variant;
}

/**
 * Starts using this item.
 * @param game the game
 */
void InventoryItem::start(Game *game) {

  this->game = game;
  this->variant = game->get_equipment()->has_inventory_item(item_id);
  this->finished = false;

  if (variant == 0) {
    DIE("Trying to use inventory item #" << item_id << " without having it");
  }

  if (is_bottle()) {
    start_bottle();
  }
  else {
    // TODO
    finished = true;
  }
}

/**
 * Updates this item when it is being used.
 */
void InventoryItem::update(void) {

  if (is_bottle()) {
    update_bottle();
  }
}

/**
 * Returns whether this item has finished to be used.
 * @return true if this item has finished to be used
 */
bool InventoryItem::is_finished(void) {
  return finished;
}

/**
 * Returns whether the specified item id corresponds to a bottle.
 * @param item_id id of an inventory item
 * @return true if it is a bottle
 */
bool InventoryItem::is_bottle(InventoryItemId item_id) {

  return item_id == INVENTORY_BOTTLE_1
    || item_id == INVENTORY_BOTTLE_2
    || item_id == INVENTORY_BOTTLE_3
    || item_id == INVENTORY_BOTTLE_4;
}

/**
 * Returns whether this item is a bottle.
 * @return true if this item is a bottle
 */
bool InventoryItem::is_bottle(void) {
  return is_bottle(item_id);
}

/**
 * Starts using this item when it is a bottle.
 */
void InventoryItem::start_bottle(void) {

  switch (variant) {

    // empty bottle
  case 1:
    {
      Detector *facing_entity = game->get_hero()->get_facing_entity();
      if (facing_entity == NULL ||
	  !facing_entity->interaction_with_inventory_item(this)) {
	
	// unless an interaction occurs, we play the "wrong" sound
	ResourceManager::get_sound("wrong")->play();
      }
      finished = true;
    }
    break;

    // water
  case 2:
    // ask the hero to pour away the water
    game->show_message("_use_bottle_with_water");
    break;

    // red potion
  case 3:
    // TODO
    finished = true;
    break;

    // green potion
  case 4:
    // TODO
    finished = true;
    break;

    // red potion
  case 5:
    // TODO
    finished = true;
    break;

    // fairy
  case 6:
    // TODO
    finished = true;
    break;

  }
}

/**
 * Updates this item when it is a bottle.
 */
void InventoryItem::update_bottle(void) {

  // bottle with water
  if (variant == 2 && !game->is_showing_message()) {

    int answer = game->get_dialog_box()->get_last_answer();

    if (answer == 0) {
      // empty the water
      game->get_equipment()->set_bottle_empty(item_id);
      ResourceManager::get_sound("item_in_water")->play();

      Detector *facing_entity = game->get_hero()->get_facing_entity();

      if (facing_entity != NULL) {
	// the player has just poured water onto an entity
	facing_entity->interaction_with_inventory_item(this);
      }
    }

    finished = true;
  }
}

