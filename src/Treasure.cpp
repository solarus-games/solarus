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
#include "lowlevel/Surface.h"

/**
 * @brief Creates a new treasure.
 * @param game the current game (cannot be NULL)
 * @param item_name name of the item to give, according to items.dat
 * ("_random" and "_none" are also accepted)
 * @param variant variant of this item
 * @param savegame_variable index of the savegame boolean indicating that the hero has found this treasure
 * or -1 if this treasure is not saved
 */
Treasure::Treasure(Game &game, const std::string &item_name, int variant, int savegame_variable):
  game(game), savegame_variable(savegame_variable), sprite(NULL) {

  std::string real_item_name;

  if (item_name == "_random") {
    game.get_equipment().get_random_item(real_item_name, variant);
  }
  else {
    real_item_name = item_name;
  }

  if (savegame_variable != -1 && game.get_savegame().get_boolean(savegame_variable)) {
    real_item_name = "_none";
  }

  this->item_name = real_item_name;
  this->variant = variant;
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
 * @brief Returns the properties of the item given with this treasure.
 * @return the item properties
 */
ItemProperties& Treasure::get_item_properties() {
  return game.get_equipment().get_item_properties(item_name);
}

/**
 * @brief Returns the name of the item.
 * @return the name of the item
 */
const std::string& Treasure::get_item_name() const {
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
  game.get_equipment().add_item(item_name, variant);

  // notify the scripts
  game.get_map_script().event_treasure_obtaining(item_name, variant, savegame_variable);
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
    sprite->set_current_animation(item_name);
    sprite->set_current_direction(variant - 1);
  }

  // display the item
  sprite->display(destination, x, y);
}

