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
#ifndef SOLARUS_ENEMY_ATTACK_H
#define SOLARUS_ENEMY_ATTACK_H

namespace Solarus {

/**
 * \brief Defines the types of attacks an enemy can be victim of.
 */
enum class EnemyAttack {
  SWORD,       /**< attacked by the sword (for a spin attack, the life lost is multiplied by 2) */
  THROWN_ITEM, /**< hit by an item thrown (bush, pot, stone...) */
  EXPLOSION,   /**< explosion of a bomb or something else */
  ARROW,       /**< hit by an arrow */
  HOOKSHOT,    /**< hit by the hookshot */
  BOOMERANG,   /**< hit by the boomerang */
  FIRE,        /**< burned by fire */
  SCRIPT       /**< a script decided to hurt the enemy */
};

}

#endif

