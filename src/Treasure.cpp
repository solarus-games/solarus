/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "EquipmentItem.h"
#include "DialogBox.h"
#include "Counter.h"
#include "Map.h"
#include "Sprite.h"
#include "lua/LuaContext.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates a new treasure.
 *
 * You must call decide_content() later because the real content of the
 * treasure may differ from the item name you specify, because of random
 * treasures and unauthorized ones.
 *
 * @param game The current game.
 * @param item_name Name of the item to give, or an empty string to mean no
 * treasure.
 * @param variant Variant of this item.
 * @param savegame_variable Name of the saved boolean indicating that the
 * player has found this treasure, or an empty string if this treasure is not
 * saved.
 */
Treasure::Treasure(Game& game, const std::string& item_name, int variant,
    const std::string& savegame_variable):
  game(&game),
  item_name(item_name),
  variant(variant),
  savegame_variable(savegame_variable),
  sprite(NULL) {

  // If the treasure is unique and was found, remove it.
  if (is_found()) {
    this->item_name = "";
    this->variant = 1;
  }
}

/**
 * @brief Copy constructor.
 * @param other The treasure to copy.
 */
Treasure::Treasure(const Treasure& other):
  game(other.game),
  item_name(other.item_name),
  variant(other.variant),
  savegame_variable(other.savegame_variable),
  sprite(NULL) {

}

/**
 * @brief Destructor.
 */
Treasure::~Treasure() {
  delete sprite;
}

/**
 * @brief Assignment operator.
 * @param other the treasure to copy
 * @return this treasure
 */
Treasure& Treasure::operator=(const Treasure& other) {

  this->game = other.game;
  this->item_name = other.item_name;
  this->variant = other.variant;
  this->savegame_variable = other.savegame_variable;
  this->sprite = NULL;
  return *this;
}

/**
 * @brief Raises an assertion error if the player cannot obtain this treasure.
 */
void Treasure::check_obtainable() const {

  Debug::check_assertion(item_name.empty()
      || game->get_equipment().get_item(item_name).is_obtainable(),
      StringConcat() << "Treasure '" << item_name
      << "' is not allowed, did you call ensure_obtainable()?");
}

/**
 * @brief Makes sure that the content of this treasure is allowed.
 *
 * If the item is not allowed, the treasure becomes empty.
 * This function must be called before any function
 * that needs to know the treasure content:
 * get_item_name(), get_item_properties(), is_empty(), give_to_player() and
 * draw().
 *
 * This function is not called automatically because we want to decide to
 * remove the treasure (or not) as late as possible. The obtainable property
 * may indeed change after the creation of the treasure.
 */
void Treasure::ensure_obtainable() {

  Equipment& equipment = game->get_equipment();
  if (!equipment.get_item(item_name).is_obtainable()) {
    item_name = "_none";
    variant = 1;
  }
}

/**
 * @brief Returns the equipment item corresponding to this treasure's content.
 * @return The equipment item.
 */
EquipmentItem& Treasure::get_item() const {
  return game->get_equipment().get_item(get_item_name());
}

/**
 * @brief Returns the name of the item.
 * @return The name of the item.
 */
const std::string& Treasure::get_item_name() const {

  check_obtainable();
  return item_name;
}

/**
 * @brief Returns the variant of the item.
 * @return The variant.
 */
int Treasure::get_variant() const {
  return variant;
}

/**
 * @brief Returns whether this treasure is saved.
 * @return true if this treasure is saved.
 */
bool Treasure::is_saved() const {
  return !get_savegame_variable().empty();
}

/**
 * @brief Returns whether the player has got this treasure according to th
 * savegame.
 *
 * Returns false if the treasure is not saved.
 *
 * @return true if the player has found this treasure.
 */
bool Treasure::is_found() const {
  return is_saved() && game->get_savegame().get_boolean(savegame_variable);
}

/**
 * @brief Returns whether this treasure is empty.
 * @return true if this treasure is empty
 */
bool Treasure::is_empty() const {
  return get_item_name() == "_none";
}

/**
 * @brief Returns the name of the boolean variable where this treasure is saved.
 * @return The savegame variable of this treasure, or an empty string if it is
 * not saved.
 */
const std::string& Treasure::get_savegame_variable() const {
  return savegame_variable;
}

/**
 * @brief Gives the treasure to the player.
 *
 * Adds the item to the hero's equipment.
 * The item should not be "_none".
 */
void Treasure::give_to_player() const {

  // Mark the treasure as found in the savegame.
  if (is_saved()) {
    game->get_savegame().set_boolean(savegame_variable, true);
  }

  // Give the item to the player.
  Equipment& equipment = game->get_equipment();
  equipment.get_item(get_item_name()).set_variant(get_variant());

  // Notify the Lua item and the Lua map.
  LuaContext& lua_context = game->get_lua_context();
  lua_context.item_on_obtaining(equipment.get_item(get_item_name()), *this);
  lua_context.hero_on_obtaining_treasure(game->get_hero(), *this);
}

/**
 * @brief Draws the treasure.
 * @param dst_surface the surface where to draw
 * @param x the treasure x position on this surface
 * @param y the treasure y position on this surface
 */
void Treasure::draw(Surface& dst_surface, int x, int y) {

  if (sprite == NULL) {
    // create the sprite only if needed (many treasures are actually never drawn)
    sprite = new Sprite("entities/items");
    sprite->set_current_animation(get_item_name());
    sprite->set_current_direction(get_variant() - 1);
  }

  // draw the item
  sprite->draw(dst_surface, x, y);
}

