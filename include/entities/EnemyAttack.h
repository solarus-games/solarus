/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_ENEMY_ATTACK_H
#define SOLARUS_ENEMY_ATTACK_H

/**
 * Defines the attacks an enemy can be victim of.
 */
enum EnemyAttack {
  ATTACK_SWORD,       /**< attacked by the sword (for a spin attack, the life lost is multiplied by 2) */
  ATTACK_THROWN_ITEM, /**< hit by an item thrown (bush, pot, stone...) */
  ATTACK_EXPLOSION,   /**< explosion of a bomb or something else */
  ATTACK_BOW,         /**< hit by an arrow */
  ATTACK_HOOKSHOT,    /**< hit by the hookshot */
  ATTACK_BOOMERANG,   /**< hit by the boomerang */
  ATTACK_LAMP,        /**< burned by the lamp */
  ATTACK_NUMBER,
};

#endif

