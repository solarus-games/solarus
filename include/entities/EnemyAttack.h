#ifndef ZSDX_ENEMY_ATTACK_H
#define ZSDX_ENEMY_ATTACK_H

/**
 * Defines the attacks an enemy can be victim of.
 */
enum EnemyAttack {
  ATTACK_SWORD,       /**< attacked by the sword (for a spin attack, the life lost is multiplied by 2) */
  ATTACK_THROWN_ITEM, /**< hit by an item thrown (bush, pot, stone...) */
  ATTACK_BOMB,        /**< explosion of a bomb */
  ATTACK_BOW,         /**< hit by an arrow */
  ATTACK_HOOKSHOT,    /**< hit by the hookshot */
  ATTACK_BOOMERANG,   /**< hit by the boomerang */
  ATTACK_LAMP,        /**< burned by the lamp */
  ATTACK_NUMBER,
};

#endif

