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
#include "Equipment.h"
#include "lua/ItemScript.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates a new inventory item.
 * @param game the game
 * @param item_name id of the item to create (according to items.dat)
 */
InventoryItem::InventoryItem(Game &game, const std::string &item_name):

  game(game),
  item_name(item_name),
  variant(game.get_equipment().get_item_variant(item_name)),
  finished(true) {

}

/**
 * @brief Destructor.
 */
InventoryItem::~InventoryItem() {

}

/**
 * @brief Returns the name of this inventory item.
 * @return the name of this inventory item
 */
const std::string& InventoryItem::get_name() {
  return item_name;
}

/**
 * @brief Returns the variant of this inventory item that the player has.
 * @return the variant
 */
int InventoryItem::get_variant() {
  return variant;
}

/**
 * @brief Starts using this item.
 */
void InventoryItem::start() {

  Debug::check_assertion(variant > 0, StringConcat() << "Trying to use inventory item '" << item_name << "' without having it");

  this->finished = false;
  game.get_equipment().get_item_script(item_name).event_use(*this);
}

/**
 * @brief Updates this item when it is being used.
 */
void InventoryItem::update() {

  // TODO?  game.get_equipment().get_item_script(item_name).event_use_update();
}

/**
 * @brief Returns whether this item has finished to be used.
 * @return true if this item has finished to be used
 */
bool InventoryItem::is_finished() {
  return finished;
}

/**
 * @brief Indicates that the player has finished using this item.
 */
void InventoryItem::set_finished() {
  this->finished = true;
}

/*
void InventoryItem::start_bottle() {

  // TODO use the script
  switch (variant) {

    // empty bottle
  case 1:
    {
      Detector *facing_entity = game.get_hero().get_facing_entity();
      if (facing_entity == NULL ||
	  !facing_entity->interaction_with_inventory_item(*this)) {

	// unless an interaction occurs, we play the "wrong" sound
	Sound::play("wrong");
      }
      finished = true;
    }
    break;

    // water
  case 2:
    // ask the hero to pour away the water
    game.get_dialog_box().start_dialog("_use_bottle_with_water");
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
    game.get_dialog_box().start_dialog("_use_bottle_with_fairy");
    break;

  }
}

void InventoryItem::update_bottle() {

  // see if a dialog is finished
  if (!game.is_showing_message()) {

    // bottle with water
    if (variant == 2) {

      int answer = game.get_dialog_box().get_last_answer();

      if (answer == 0) {
	// empty the water
	game.get_equipment().set_item_variant(item_name, 1);
	Sound::play("item_in_water");

	Detector *facing_entity = game.get_hero().get_facing_entity();

	if (facing_entity != NULL) {
	  // the player has just poured water onto an entity
	  facing_entity->interaction_with_inventory_item(*this);
	}
      }
      finished = true;
    }

    // bottle with a fairy
    else if (variant == 6) {

      int answer = game.get_dialog_box().get_last_answer();

      if (answer == 1) {
	// release the fairy
	Map &map = game.get_current_map();
	Hero &hero = game.get_hero();
	map.get_entities().add_entity(PickableItem::create(
	      game, hero.get_layer(), hero.get_x(), hero.get_y(),
	      Treasure(game, "fairy", 1, -1),
	      FALLING_LOW, true));
	game.get_equipment().set_item_variant(item_name, 1);

      }
      finished = true;
    }
  }
}
*/

/**
 * @brief Sets the current map.
 *
 * This function is called when the map is changed while the player is still using this item.
 *
 * @param map the map
 */
void InventoryItem::set_map(Map &map) {
  
}

