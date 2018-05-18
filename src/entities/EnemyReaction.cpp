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
#include "solarus/entities/EnemyReaction.h"
#include "solarus/graphics/Sprite.h"
#include <sstream>

namespace Solarus {

const std::string EnumInfoTraits<EnemyReaction::ReactionType>::pretty_name = "enemy reaction type";

const EnumInfo<EnemyReaction::ReactionType>::names_type EnumInfoTraits<EnemyReaction::ReactionType>::names = {
  { EnemyReaction::ReactionType::HURT, "hurt" },
  { EnemyReaction::ReactionType::IGNORED, "ignored" },
  { EnemyReaction::ReactionType::PROTECTED, "protected" },
  { EnemyReaction::ReactionType::IMMOBILIZED, "immobilized" },
  { EnemyReaction::ReactionType::CUSTOM, "custom" }
};

/**
 * \brief Constructor.
 */
EnemyReaction::EnemyReaction() {

  set_default_reaction();
}

/**
 * \brief Resets this reaction to its default value.
 */
void EnemyReaction::set_default_reaction() {

  general_reaction.type = ReactionType::IGNORED;
  general_reaction.life_lost = 0;
  sprite_reactions.clear();
}

/**
 * \brief Sets how the enemy reacts when the sprite that receives the attack
 * has no specific reaction.
 * \param reaction The reaction to set.
 * \param life_lost Number of life points to remove (only for reaction HURT).
 * \param callback Lua function to call (only for reaction LUA_CALLBACK).
 */
void EnemyReaction::set_general_reaction(ReactionType reaction, int life_lost, const ScopedLuaRef& callback) {

  general_reaction.type = reaction;
  if (reaction == ReactionType::HURT) {
    if (life_lost < 0) {
      std::ostringstream oss;
      oss << "Invalid amount of life: " << life_lost;
      Debug::die(oss.str());
    }
    general_reaction.life_lost = life_lost;
  }
  else if (reaction == ReactionType::LUA_CALLBACK) {
    Debug::check_assertion(!callback.is_empty(), "Missing enemy reaction callback");
    general_reaction.callback = callback;
  }
}

/**
 * \brief Makes the enemy reacts differently when the attack is received by a particular sprite.
 * \param sprite A sprite of the enemy (if nullptr, this is equivalent to set_general_reaction).
 * \param reaction The reaction to make when this specific sprites receives an attack.
 * \param life_lost Number of life points to remove (only for reaction HURT).
 * \param callback Lua function to call (only for reaction LUA_CALLBACK).
 */
void EnemyReaction::set_sprite_reaction(const Sprite* sprite, ReactionType reaction, int life_lost, const ScopedLuaRef& callback) {

  if (sprite == nullptr) {
    set_general_reaction(reaction, life_lost);
  }
  else {
    sprite_reactions[sprite].type = reaction;
    if (reaction == ReactionType::HURT) {
      if (life_lost < 0) {
        std::ostringstream oss;
        oss << "Invalid amount of life: " << life_lost;
        Debug::die(oss.str());
      }
      sprite_reactions[sprite].life_lost = life_lost;
    }
    else if (reaction == ReactionType::LUA_CALLBACK) {
      Debug::check_assertion(!callback.is_empty(), "Missing enemy reaction callback");
      sprite_reactions[sprite].callback = callback;
    }
  }
}

/**
 * \brief Returns the reaction to an attack on a sprite.
 * \param sprite the sprite that receives the attack
 * \return the corresponding reaction
 */
const EnemyReaction::Reaction& EnemyReaction::get_reaction(
    const Sprite* sprite) const {

  if (sprite != nullptr) {
    auto it = sprite_reactions.find(sprite);
    if (it != sprite_reactions.end()) {
      return it->second;
    }
  }
  return general_reaction;
}

}
