#ifndef ZSDX_ENEMY_H
#define ZSDX_ENEMY_H

#include "Common.h"

/**
 * Represents an enemy.
 * For each subtype of enemy, a subclass determines its behavior.
 */
class Enemy {

 public:

  /**
   * Types of enemies.
   */
  enum EnemyType {

    GREEN_SOLDIER
  };

 protected:

  /**
   * This structure defines the properties of a type of  enemy.
   */
  struct Properties {
    SpriteAnimationSetId animation_set_id; /**< animation set used for this type of enemy */
    int initial_life;                      /**< initial health points of the enemy */
    int damage_on_hero;                    /**< */
  };
  
};

#endif
