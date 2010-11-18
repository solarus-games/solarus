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
#include "Treasure.h"
#include "Game.h"
#include "DialogBox.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "Counter.h"
#include "Map.h"
#include "Sprite.h"
#include "lua/MapScript.h"
#include "lua/ItemScript.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"

/**
 * @brief Creates a new treasure.
 * @param game the current game (cannot be NULL)
 * @param item_name name of the item to give, according to items.dat
 * ("_random" and "_none" are also accepted; if you specify "_random", you must call
 * decide_content() later)
 * @param variant variant of this item
 * @param savegame_variable index of the savegame boolean indicating that the hero has found this treasure
 * or -1 if this treasure is not saved
 */
Treasure::Treasure(Game &game, const std::string &item_name, int variant, int savegame_variable):
  game(game),
  item_name(item_name),
  variant(variant),
  savegame_variable(savegame_variable),
  sprite(NULL) {

  Equipment &equipment = game.get_equipment();

  // check that the item is authorized
  if (item_name != "_none" && item_name != "_random"
      && !equipment.can_receive_item(item_name, variant)) {
    this->item_name = "_none";
    this->variant = 1;
  }

  // if the treasure is unique, check its state
  if (savegame_variable != -1 && game.get_savegame().get_boolean(savegame_variable)) {
    this->item_name = "_none";
    this->variant = 1;
  }
}

/**
 * @brief Copy constructor.
 * @param other the treasure to copy
 */
Treasure::Treasure(const Treasure &other):
  game(other.game), item_name(other.item_name), variant(other.variant),
  savegame_variable(other.savegame_variable), sprite(NULL) {

}

/**
 * @brief Destructor.
 */
Treasure::~Treasure() {
  delete sprite;
}

/**
 * @brief If the treasure is "_random", chooses a random item and variant according to the probabilities of items.dat.
 *
 * If the item is "_random", this function must be called before any function
 * that needs to know the treasure content:
 * get_item_name(), get_item_properties(), is_empty(), give_to_player() and display().
 * If the item is not "_random", this function has no effect.
 */
void Treasure::decide_content() {

  Equipment &equipment = game.get_equipment();
  if (item_name == "_random") {
    // choose a random item
    equipment.get_random_item(item_name, variant);

    // check that the item is authorized
    if (item_name != "_none"
        && !equipment.can_receive_item(item_name, variant)) {
      item_name = "_none";
      variant = 1;
    }
  }
}

/**
 * @brief Returns the properties of the item given with this treasure.
 * @return the item properties
 */
ItemProperties& Treasure::get_item_properties() const {
  return game.get_equipment().get_item_properties(get_item_name());
}

/**
 * @brief Returns the name of the item.
 * @return the name of the item
 */
const std::string& Treasure::get_item_name() const {

  Debug::assert(item_name != "_random", "This treasure has a random content and it is not decided yet");

  return item_name;
}

/**
 * @brief Returns the variant of the item.
 * @return the variant
 */
int Treasure::get_variant() const {
  return variant;
}

/**
 * @brief Returns whether this treasure is saved.
 * @return true if this treasure is saved
 */
bool Treasure::is_saved() const {
  return get_savegame_variable() != -1;
}

/**
 * @brief Returns whether the player has got this treasure according to the savegame.
 *
 * Returns false if the treasure possession state is not saved.
 *
 * @return true if the player has found this treasure
 */
bool Treasure::is_found() const {
  return savegame_variable != -1 && game.get_savegame().get_boolean(savegame_variable);
}

/**
 * @brief Returns whether this treasure is empty.
 * @return true if this treasure is empty
 */
bool Treasure::is_empty() const {
  return get_item_name() == "_none";
}

/**
 * @brief Returns the index of the variable where this treasure is saved.
 * @return the savegame variable of this treasure, or -1 if it is not saved
 */
int Treasure::get_savegame_variable() const {
  return savegame_variable;
}

/**
 * @brief Gives the treasure to the player.
 *
 * Adds the item to the hero's equipment.
 * The item should not be "_none".
 */
void Treasure::give_to_player() const {

  // mark the treasure as found in the savegame
  if (savegame_variable != -1) {
    game.get_savegame().set_boolean(savegame_variable, true);
  }

  // give the item
  Equipment &equipment = game.get_equipment();
  equipment.add_item(get_item_name(), get_variant());

  // notify the scripts
  equipment.get_item_script(get_item_name()).event_obtaining(*this);
  game.get_map_script().event_treasure_obtaining(*this);
}

/**
 * @brief Displays the treasure.
 * @param destination the surface where to draw
 * @param x the treasure x position on this surface
 * @param y the treasure y position on this surface
 */
void Treasure::display(Surface *destination, int x, int y) {

  if (sprite == NULL) {
    // create the sprite only if needed (many treasures are actually never displayed)
    sprite = new Sprite("entities/items");
    sprite->set_current_animation(get_item_name());
    sprite->set_current_direction(get_variant() - 1);
  }

  // display the item
  sprite->display(destination, x, y);
}

