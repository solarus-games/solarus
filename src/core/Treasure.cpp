/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/core/Equipment.h"
#include "solarus/core/EquipmentItem.h"
#include "solarus/core/Game.h"
#include "solarus/core/Map.h"
#include "solarus/core/Savegame.h"
#include "solarus/core/Treasure.h"
#include "solarus/graphics/Sprite.h"
#include "solarus/graphics/Surface.h"
#include "solarus/lua/LuaContext.h"
#include <memory>

namespace Solarus {

/**
 * \brief Creates a new treasure.
 *
 * You must call ensure_obtainable() before giving it to the player, because
 * of unauthorized treasures.
 *
 * \param game The current game.
 * \param item_name Name of the item to give, or an empty string to mean no
 * treasure.
 * \param variant Variant of this item.
 * \param savegame_variable Name of the saved boolean indicating that the
 * player has found this treasure, or an empty string if this treasure is not
 * saved.
 */
Treasure::Treasure(Game& game, const std::string& item_name, int variant,
    const std::string& savegame_variable):
  game(&game),
  item_name(item_name),
  variant(variant),
  savegame_variable(savegame_variable) {

}

/**
 * \brief Returns whether the player can obtain this treasure.
 */
bool Treasure::is_obtainable() const {

  return item_name.empty()
      || game->get_equipment().get_item(item_name).is_obtainable();
}

/**
 * \brief Raises an assertion error if the player cannot obtain this treasure.
 */
void Treasure::check_obtainable() const {

  if (!is_obtainable()) {
    Debug::die(std::string("Treasure '") + item_name
      + "' is not allowed, did you call ensure_obtainable()?");
  }
}

/**
 * \brief Makes sure that the content of this treasure is allowed.
 *
 * If the item is not allowed, the treasure becomes empty.
 * This function must be called before giving it to the player.
 *
 * This function is not called automatically because we want to decide to
 * remove the treasure (or not) as late as possible. The obtainable property
 * may indeed change after the creation of the treasure, for example if the
 * player finds a new equipment item in the meantime.
 */
void Treasure::ensure_obtainable() {

  if (!is_obtainable()) {
    item_name = "";
    variant = 1;
  }
}

/**
 * \brief Returns the game where this treasure was created.
 * \return The game.
 */
Game& Treasure::get_game() const {
  return *game;
}

/**
 * \brief Returns the equipment item corresponding to this treasure's content.
 * \return The equipment item.
 */
EquipmentItem& Treasure::get_item() const {
  return game->get_equipment().get_item(get_item_name());
}

/**
 * \brief Returns the name of the item.
 * \return The name of the item.
 */
const std::string& Treasure::get_item_name() const {
  return item_name;
}

/**
 * \brief Returns the variant of the item.
 * \return The variant.
 */
int Treasure::get_variant() const {
  return variant;
}

/**
 * \brief Returns whether this treasure is saved.
 * \return \c true if this treasure is saved.
 */
bool Treasure::is_saved() const {
  return !get_savegame_variable().empty();
}

/**
 * \brief Returns whether the player has got this treasure according to th
 * savegame.
 *
 * Returns false if the treasure is not saved.
 *
 * \return \c true if the player has found this treasure.
 */
bool Treasure::is_found() const {
  return is_saved() && game->get_savegame().get_boolean(savegame_variable);
}

/**
 * \brief Returns whether this treasure is empty.
 * \return \c true if this treasure is empty.
 */
bool Treasure::is_empty() const {
  return get_item_name().empty();
}

/**
 * \brief Returns the name of the boolean variable where this treasure is saved.
 * \return The savegame variable of this treasure, or an empty string if it is
 * not saved.
 */
const std::string& Treasure::get_savegame_variable() const {
  return savegame_variable;
}

/**
 * \brief Gives the treasure to the player.
 *
 * Adds the item to the hero's equipment.
 * The item should not be empty and must be obtainable.
 */
void Treasure::give_to_player() const {

  check_obtainable();

  // Mark the treasure as found in the savegame.
  if (is_saved()) {
    game->get_savegame().set_boolean(savegame_variable, true);
  }

  // Give the item to the player.
  EquipmentItem& item = get_item();
  if (item.is_saved()) {
    item.set_variant(get_variant());
  }

  // Notify the Lua item and the Lua map.
  LuaContext& lua_context = game->get_lua_context();
  lua_context.item_on_obtaining(item, *this);
  lua_context.map_on_obtaining_treasure(game->get_current_map(), *this);
}

/**
 * \brief Creates and returns a sprite representing this treasure.
 */
SpritePtr Treasure::create_sprite() const {

  SpritePtr sprite = std::make_shared<Sprite>("entities/items");
  sprite->set_current_animation(get_item_name());
  sprite->set_current_direction(get_variant() - 1);
  return sprite;
}

}

