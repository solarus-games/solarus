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
#include "entities/EnemyReaction.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Sprite.h"

const std::string EnemyReaction::reaction_names[REACTION_NUMBER] = {
  "hurt",
  "ignored",
  "protected",
  "immobilized",
  "custom"
};

/**
 * @brief Constructor.
 */
EnemyReaction::EnemyReaction() {

  set_default_reaction();
}

/**
 * @brief Destructor.
 */
EnemyReaction::~EnemyReaction() {

}

/**
 * @brief Resets this reaction to its default value.
 */
void EnemyReaction::set_default_reaction() {

  general_reaction.type = IGNORED;
  general_reaction.life_lost = 0;
  sprite_reactions.clear();
}

/**
 * @brief Sets how the enemy reacts when the sprite that receives the attack
 * has no specific reaction.
 * @param reaction the reaction to set
 * @param life_lost number of life points to remove (only for reaction HURT)
 */
void EnemyReaction::set_general_reaction(ReactionType reaction, int life_lost) {

  general_reaction.type = reaction;
  if (reaction == HURT) {
    general_reaction.life_lost = life_lost;
  }
}

/**
 * @brief Makes the enemy reacts differently when the attack is received by a particular sprite.
 * @param sprite a sprite of the enemy (if NULL, this is equivalent to set_general_reaction)
 * @param reaction the reaction to make when this specific sprites receives an attack
 * @param life_lost number of life points to remove (only for reaction HURT)
 */
void EnemyReaction::set_sprite_reaction(const Sprite* sprite, ReactionType reaction, int life_lost) {

  if (sprite == NULL) {
    set_general_reaction(reaction, life_lost);
  }
  else {
    const std::string& name = sprite->get_animation_set_id();
    sprite_reactions[name].type = reaction;
    sprite_reactions[name].life_lost = life_lost;
  }
}

/**
 * @brief Returns the reaction to an attack on a sprite.
 * @param sprite the sprite that receives the attack
 * @return the corresponding reaction
 */
const EnemyReaction::Reaction& EnemyReaction::get_reaction(const Sprite* sprite) {

  if (sprite != NULL) {
    const std::string& name = sprite->get_animation_set_id();
    if (sprite_reactions.count(name) > 0) {
      return sprite_reactions[name];
    }
  }
  return general_reaction;
}

/**
 * @brief Returns the name corresponding to a type of reaction.
 * @param reaction a type of reaction
 * @return the corresponding name
 */
const std::string& EnemyReaction::get_reaction_name(ReactionType reaction) {

  Debug::check_assertion(reaction >= 0 && reaction < REACTION_NUMBER,
      StringConcat() << "Invalid reaction number: " << reaction);

  return reaction_names[reaction];
}

/**
 * @brief Returns a type of reaction from its name.
 * @param name a name of reaction type
 * @return the corresponding reaction type
 */
EnemyReaction::ReactionType EnemyReaction::get_reaction_by_name(const std::string& name) {

  for (int i = 0; i < REACTION_NUMBER; i++) {
    if (reaction_names[i] == name) {
      return ReactionType(i);
    }
  }

  Debug::die(StringConcat() << "Invalid reaction name: " << name);
  throw;
}

