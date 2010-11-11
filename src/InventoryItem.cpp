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
#include "ItemProperties.h"
#include "Treasure.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "Map.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "entities/Boomerang.h"
#include "entities/Arrow.h"
#include "entities/PickableItem.h"
#include "hero/HeroSprites.h"
#include "movements/FallingHeight.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Geometry.h"

/**
 * @brief Creates a new inventory item.
 * @param game the game
 * @param item_name id of the item to create (according to items.dat)
 */
InventoryItem::InventoryItem(Game &game, const std::string &item_name):

  game(game),
  item_name(item_name),
  variant(game.get_equipment().get_item_variant(item_name)) {

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

  Hero &hero = game.get_hero();
  Equipment &equipment = game.get_equipment();

  this->finished = false;
  this->item_sound_id = "";

  Debug::assert(variant > 0, StringConcat() << "Trying to use inventory item '" << item_name << "' without having it");

  // TODO use scripts
  if (item_name.substr(0, 7) == "bottle_") {
    start_bottle();
  }
  else {

    if (item_name == "boomerang") {
      hero.start_boomerang();
      finished = true;
    }
    else if (item_name == "bow") {

      if (equipment.get_item_amount("bow") == 0) {
	Sound::play("wrong");
	finished = true;
      }
      else {
	equipment.remove_item_amount("bow", 1);
        hero.start_bow();
      }
      finished = true;
    }
    else if (item_name == "pegasus_shoes") {

      hero.start_running();
      finished = true;
    }
    else if (item_name == "apples") {
      if (equipment.get_item_amount(item_name) == 0) {
	Sound::play("wrong");
	finished = true;
      }
      else {
	game.get_dialog_box().start_dialog("_use_apples");
      }
    }
    else if (item_name == "pains_au_chocolat") {
      if (equipment.get_item_amount(item_name) == 0) {
	Sound::play("wrong");
	finished = true;
      }
      else {
	game.get_dialog_box().start_dialog("_use_pains_au_chocolat");
      }
    }
    else if (item_name == "croissants") {
      if (equipment.get_item_amount(item_name) == 0) {
	Sound::play("wrong");
	finished = true;
      }
      else {
	game.get_dialog_box().start_dialog("_use_croissants");
      }
    }
    else {
      finished = true;
    }
  }
}

/**
 * @brief Updates this item when it is being used.
 */
void InventoryItem::update() {

  Equipment &equipment = game.get_equipment();

  if (item_sound_id.size() != 0) {
    uint32_t now = System::now();
    if (now >= next_sound_date) {
      Sound::play(item_sound_id);
      next_sound_date = now + sound_delay;
    }
  }

  // TODO use scripts
  if (item_name.substr(0, 7) == "bottle_") {
    update_bottle();
  }
  else {

    if (item_name == "apples" || item_name == "pains_au_chocolat" || item_name == "croissants") {

      if (!game.is_showing_message()) {

	if (game.get_dialog_box().get_last_answer() == 0 &&
	    equipment.get_item_amount(item_name) > 0) {

	  equipment.remove_item_amount(item_name, 1);
	  int nb_hearts =
	    (item_name == "apples") ? 1 :
	    (item_name == "pains_au_chocolat") ? 3 : 7;
	  equipment.add_life(nb_hearts * 4);
	}
	finished = true;
      }
    }
  }
}

/**
 * @brief Returns whether this item has finished to be used.
 * @return true if this item has finished to be used
 */
bool InventoryItem::is_finished() {
  return finished;
}

/**
 * @brief Starts using this item when it is a bottle.
 */
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

/**
 * @brief Updates this item when it is a bottle.
 */
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

/**
 * @brief Sets the current map.
 *
 * This function is called when the map is changed while the player is still using this item.
 *
 * @param map the map
 */
void InventoryItem::set_map(Map &map) {
  
}

