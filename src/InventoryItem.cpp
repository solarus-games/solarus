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
#include "Map.h"
#include "Controls.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "entities/Detector.h"
#include "entities/Boomerang.h"

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
  return item_id < INVENTORY_ROCK_KEY || is_bottle(item_id);
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
  this->item_sound = NULL;

  Hero *hero = game->get_hero();
  Map *map = game->get_current_map();
  Equipment *equipment = game->get_equipment();

  if (variant == 0) {
    DIE("Trying to use inventory item #" << item_id << " without having it");
  }

  if (is_bottle()) {
    start_bottle();
  }
  else {
    switch (item_id) {

      case INVENTORY_BOOMERANG:

	if (map->get_entities()->is_boomerang_present()) {
	  finished = true;
	}
	else {
	  hero->set_animation_boomerang();
	  this->direction_pressed = game->get_controls()->get_arrows_direction();
	}
	break;
   
      case INVENTORY_APPLES:
      case INVENTORY_PAINS_AU_CHOCOLAT:
      case INVENTORY_CROISSANTS:
        if (equipment->get_inventory_item_amount(item_id) == 0) {
          ResourceManager::get_sound("wrong")->play();
	  finished = true;
	}
	else {
	  MessageId message_id =
	    (item_id == INVENTORY_APPLES) ? "_use_apples" :
	    (item_id == INVENTORY_PAINS_AU_CHOCOLAT) ? "_use_pains_au_chocolat" : "_use_croissants";

          game->show_message(message_id);
	}
	break;

      default:
	// TODO
	finished = true;
	break;
    }
  }
}

/**
 * Updates this item when it is being used.
 */
void InventoryItem::update(void) {

  Hero *hero = game->get_hero();
  Equipment *equipment = game->get_equipment();

  if (item_sound != NULL) {
    Uint32 now = SDL_GetTicks();
    if (now >= next_sound_date) {
      item_sound->play();
      next_sound_date = now + sound_delay;
    }
  }

  if (is_bottle()) {
    update_bottle();
  }
  else {
    
    switch (item_id) {
      
      case INVENTORY_BOOMERANG:
	if (hero->is_animation_finished()) {
	  finished = true;
	  int boomerang_direction = direction_pressed;
	  if (boomerang_direction == -1) {
	    boomerang_direction = hero->get_animation_direction() * 90;
	  }
	  game->get_current_map()->get_entities()->add_entity(new Boomerang(hero, boomerang_direction));
	}
	break;

      case INVENTORY_APPLES:
      case INVENTORY_PAINS_AU_CHOCOLAT:
      case INVENTORY_CROISSANTS:

        if (!game->is_showing_message()) {
	  
	  if (DialogBox::get_last_answer() == 0 &&
	      equipment->get_inventory_item_amount(item_id) >= 0) {

	    equipment->remove_inventory_item_amount(item_id, 1);
	    int nb_hearts =
	      (item_id == INVENTORY_APPLES) ? 1 :
	      (item_id == INVENTORY_PAINS_AU_CHOCOLAT) ? 3 : 7;
	    equipment->add_hearts(nb_hearts * 4);
	  }
	  finished = true;
	}
	break;

      default:
	break;
    }
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

/**
 * Sets the current map.
 * This function is called when the map is changed while the player is still using this item.
 */
void InventoryItem::set_map(Map *map) {
  // TODO boomerang: put the boomerang in the new map  
}

