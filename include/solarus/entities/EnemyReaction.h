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
#ifndef SOLARUS_ENEMY_REACTION_H
#define SOLARUS_ENEMY_REACTION_H

#include "solarus/core/Common.h"
#include "solarus/core/EnumInfo.h"
#include "solarus/lua/ScopedLuaRef.h"
#include <map>
#include <string>

namespace Solarus {

class Sprite;

/**
 * \brief Describes how an enemy reacts when it receives an attack.
 *
 * The reaction may be different between different sprites of the enemy.
 */
class SOLARUS_API EnemyReaction {

  public:

    /**
     * \brief Types of reactions to an attack.
     */
    enum class ReactionType {
      HURT,              /**< The enemy is hurt and loses some life points. */
      IGNORED,           /**< Nothing happens. */
      PROTECTED,         /**< The attack is stopped. */
      IMMOBILIZED,       /**< The enemy is temporarily immobilized. */
      CUSTOM,            /**< enemy:on_custom_attack_received is called */
      LUA_CALLBACK       /**< A Lua function is called. */
    };

    /**
     * \brief Represents a reaction to an attack.
     */
    struct Reaction {
      ReactionType type;      /**< Type of reaction. */
      int life_lost;          /**< Number of life points lost, possibly zero
                               * (with reaction type CUSTOM). */
      ScopedLuaRef callback;  /**< Lua function to call
                               * (with reaction type LUA_CALLBACK). */

      Reaction() :
        type(ReactionType::IGNORED),
        life_lost(0),
        callback() {
      }
    };

    EnemyReaction();

    void set_default_reaction();
    void set_general_reaction(ReactionType reaction, int life_lost = 0, const ScopedLuaRef& callback = ScopedLuaRef());
    void set_sprite_reaction(const Sprite* sprite, ReactionType reaction, int life_lost = 0, const ScopedLuaRef& callback = ScopedLuaRef());
    const Reaction& get_reaction(const Sprite* sprite) const;

  private:

    Reaction general_reaction;                             /**< Reaction to make unless there are sprite-specific overrides. */
    std::map<const Sprite*, Reaction> sprite_reactions;    /**< Sprite-specific reaction (overrides the default one). */

};

template <>
struct SOLARUS_API EnumInfoTraits<EnemyReaction::ReactionType> {
  static const std::string pretty_name;

  static const EnumInfo<EnemyReaction::ReactionType>::names_type names;
};

}

#endif

