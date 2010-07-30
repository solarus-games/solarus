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
#include "InventoryItem.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "MapScript.h"
#include "Map.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "entities/Detector.h"
#include "entities/Boomerang.h"
#include "entities/Arrow.h"
#include "entities/PickableItem.h"
#include "movements/FallingHeight.h"
#include "lowlevel/System.h"

/**
 * @brief Creates a new inventory item.
 * @param item_id id of the item to create
 */
InventoryItem::InventoryItem(InventoryItemId item_id):
  item_id(item_id) {

}

/**
 * @brief Destructor.
 */
InventoryItem::~InventoryItem(void) {

}

/**
 * @brief Returns whether the specified item can be assigned to icon X or V.
 * @param item_id id of a item
 * @return true if this item item can be assigned to an icon
 */
bool InventoryItem::can_be_assigned(InventoryItemId item_id) {
  return item_id < INVENTORY_ROCK_KEY || is_bottle(item_id);
}

/**
 * @brief Returns whether a counter is associated to the specified item.
 *
 * This is equivalent to get_counter_index(item_id) != -1.
 *
 * @param item_id id of an item
 * @return true if this item has a counter
 */
bool InventoryItem::has_counter(InventoryItemId item_id) {
  return get_counter_index(item_id) != -1;
}

/**
 * @brief Returns the index of the savegame variable where the counter of this item is stored.
 *
 * If the specified item has a counter, returns the index of the savegame
 * variable indicating the counter's value. Otherwise, returns -1.
 *
 * @param item_id an inventory item (with or without counter)
 * @return the index of the savegame variable indicating the counter's value,
 * or -1 if there is no counter on this item
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
 * @brief Returns the id of this inventory item.
 * @return the id of this inventory item
 */
InventoryItemId InventoryItem::get_id(void) {
  return item_id;
}

/**
 * @brief Returns the variant of this inventory item that the player has.
 * @return the variant
 */
int InventoryItem::get_variant(void) {
  return variant;
}

/**
 * @brief Starts using this item.
 * @param game the game
 */
void InventoryItem::start(Game *game) {

  this->game = game;
  this->variant = game->get_equipment()->has_inventory_item(item_id);
  this->finished = false;
  this->item_sound_id = "";

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
	  hero->start_boomerang();
	  this->direction_pressed = game->get_controls()->get_arrows_direction();
	}
	break;
   
      case INVENTORY_BOW:
	if (equipment->get_arrows() == 0) {
          game->play_sound("wrong");
	  finished = true;
	}
        else {
	  equipment->remove_arrow();
	  hero->start_bow();
	}
	break;

      case INVENTORY_SPEED_SHOES:
        hero->start_running();
	break;

      case INVENTORY_APPLES:
      case INVENTORY_PAINS_AU_CHOCOLAT:
      case INVENTORY_CROISSANTS:
        if (equipment->get_inventory_item_amount(item_id) == 0) {
          game->play_sound("wrong");
	  finished = true;
	}
	else {
	  MessageId message_id =
	    (item_id == INVENTORY_APPLES) ? "_use_apples" :
	    (item_id == INVENTORY_PAINS_AU_CHOCOLAT) ? "_use_pains_au_chocolat" : "_use_croissants";

          game->get_dialog_box()->start_dialog(message_id);
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
 * @brief Updates this item when it is being used.
 */
void InventoryItem::update(void) {

  Hero *hero = game->get_hero();
  Equipment *equipment = game->get_equipment();

  if (item_sound_id.size() != 0) {
    uint32_t now = System::now();
    if (now >= next_sound_date) {
      game->play_sound(item_sound_id);
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

	  if (direction_pressed == -1) {
	    // the player can press the diagonal arrows before or after the boomerang key
	    direction_pressed = game->get_controls()->get_arrows_direction();
	  }
	  int boomerang_direction = direction_pressed;
	  if (boomerang_direction == -1 || boomerang_direction % 90 == 0) {
	    boomerang_direction = hero->get_animation_direction() * 90;
	  }
	  game->get_current_map()->get_entities()->add_entity(new Boomerang(hero, boomerang_direction));
	}
	break;
 
      case INVENTORY_BOW:
	if (hero->is_animation_finished()) {
	  finished = true;
	  game->get_current_map()->get_entities()->add_entity(new Arrow(hero));
          game->play_sound("bow");
	}
	break;
 
      case INVENTORY_SPEED_SHOES:
	// it's immediately finished for us : from the point of view of the InventoryItem class,
	// the only effect of the speed shoes is to make the hero take its state "running"
	finished = true;
	break;

      case INVENTORY_APPLES:
      case INVENTORY_PAINS_AU_CHOCOLAT:
      case INVENTORY_CROISSANTS:

        if (!game->is_showing_message()) {
	  
	  if (game->get_dialog_box()->get_last_answer() == 0 &&
	      equipment->get_inventory_item_amount(item_id) > 0) {

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
 * @brief Returns whether this item has finished to be used.
 * @return true if this item has finished to be used
 */
bool InventoryItem::is_finished(void) {
  return finished;
}

/**
 * @brief Returns whether the specified item id corresponds to a bottle.
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
 * @brief Returns whether this item is a bottle.
 * @return true if this item is a bottle
 */
bool InventoryItem::is_bottle(void) {
  return is_bottle(item_id);
}

/**
 * @brief Starts using this item when it is a bottle.
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
	game->play_sound("wrong");
      }
      finished = true;
    }
    break;

    // water
  case 2:
    // ask the hero to pour away the water
    game->get_dialog_box()->start_dialog("_use_bottle_with_water");
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
    // ask the hero to release the fairy
    game->get_dialog_box()->start_dialog("_use_bottle_with_fairy");
    break;

  }
}

/**
 * @brief Updates this item when it is a bottle.
 */
void InventoryItem::update_bottle(void) {

  // see if a dialog is finished
  if (!game->is_showing_message()) {

    // bottle with water
    if (variant == 2) {

      int answer = game->get_dialog_box()->get_last_answer();

      if (answer == 0) {
	// empty the water
	game->get_equipment()->set_bottle_empty(item_id);
	game->play_sound("item_in_water");

	Detector *facing_entity = game->get_hero()->get_facing_entity();

	if (facing_entity != NULL) {
	  // the player has just poured water onto an entity
	  facing_entity->interaction_with_inventory_item(this);
	}
      }
      finished = true;
    }

    // bottle with a fairy
    else if (variant == 6) {
      
      int answer = game->get_dialog_box()->get_last_answer();

      if (answer == 1) {
	// release the fairy
	Map *map = game->get_current_map();
	Hero *hero = game->get_hero();
	map->get_entities()->add_entity(PickableItem::create(game, hero->get_layer(), hero->get_x(), hero->get_y(),
	      PickableItem::FAIRY, -1, FALLING_LOW, true));
	game->get_equipment()->set_bottle_empty(item_id);

      }
      finished = true;
    }
  }
}

/**
 * @brief Sets the current map.
 *
 * This function is called when the map is changed while the player is still using this item.
 *
 * @param map the map
 */
void InventoryItem::set_map(Map *map) {
  
}

