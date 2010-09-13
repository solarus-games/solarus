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
#include "MapScript.h"
#include "Sprite.h"
#include "lowlevel/Surface.h"

/**
 * @brief Creates a new treasure.
 * @param game the current game (cannot be NULL)
 * @param item_name name of the item to give, according to items.dat
 * @param variant variant of this item
 * @param savegame_variable index of the savegame boolean indicating that the hero has found this treasure
 * or -1 if this treasure is not saved
 */
Treasure::Treasure(Game *game, const std::string &item_name, int variant, int savegame_variable):
  game(game), item_name(item_name), variant(variant), savegame_variable(savegame_variable), sprite(NULL) {

}

/**
 * @brief Destructor.
 */
Treasure::~Treasure(void) {
  delete sprite;
}

/**
 * @brief Creates a new treasure.
 *
 * This method acts like a constructor, except that it returns NULL in the following cases:
 * - the item is "_none",
 * or:
 * - the item is "_random" and the random choice selects "_none",
 * or:
 * - the item is saved and the player already has it.
 *
 * @param game the current game (cannot be NULL)
 * @param item_name name of the item to give, according to items.dat
 * ("_random" and "_none" are also accepted)
 * @param variant variant of this item
 * @param savegame_variable index of the savegame boolean indicating that the hero has found this treasure
 * or -1 if this treasure is not saved
 */
Treasure * Treasure::create(Game *game, std::string item_name, int variant, int savegame_variable) {

  if (item_name == "_random") {
    item_name = game->get_equipment()->get_random_item();
  }

  if (item_name == "_none") {
    return NULL;
  }

  if (game->get_savegame()->get_boolean(savegame_variable)) {
    return NULL;
  }

  return new Treasure(game, item_name, variant, savegame_variable);
}

/**
 * @brief Returns the properties of the item given with this treasure.
 * @return the item properties
 */
ItemProperties * Treasure::get_item_properties(void) {
  return game->get_equipment()->get_item_properties(item_name);
}

/**
 * @brief Returns the name of the item.
 * @return the name of the item
 */
const std::string & Treasure::get_item_name(void) {
  return item_name;
}

/**
 * @brief Returns the variant of the item.
 * @return the variant
 */
int Treasure::get_variant(void) {
  return variant;
}

/**
 * @brief Returns whether this treasure is saved.
 * @return true if this treasure is saved
 */
bool Treasure::is_saved(void) {
  return get_savegame_variable() != -1;
}

/**
 * @brief Returns the index of the variable where this treasure is saved.
 * @return the savegame variable of this treasure, or -1 if it is not saved
 */
int Treasure::get_savegame_variable(void) {
  return savegame_variable;
}

/**
 * @brief Give the treasure to the player.
 *
 * Plays the treasure sound, makes the hero
 * brandish the item and adds the item to the hero's equipment.
 * The treasure content should not be NONE.
 */
void Treasure::give_to_player(void) {

  show_message();
  add_item_to_equipment();
  game->get_current_script()->event_treasure_obtaining(item_name, variant, savegame_variable);
}

/**
 * @brief Shows the message telling that the player
 * has found the item.
 */
void Treasure::show_message(void) {

  // the message id is _treasure.x.y where x is the item name and y is the variant
  std::ostringstream oss;
  oss << "_treasure." << item_name << "." << variant;
  game->get_dialog_box()->start_dialog(oss.str());
}

/**
 * @brief Adds the item to the hero's equipment and sets this treasure as obtained.
 */
void Treasure::add_item_to_equipment(void) {

  // mark the treasure as found in the savegame
  if (savegame_variable != -1) {
    game->get_savegame()->set_boolean(savegame_variable, true);
  }

  // give the item
  game->get_equipment()->add_item(item_name, variant);
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
    sprite->set_current_direction(variant);
  }

  // display the item
  sprite->display(destination, x, y);
}

