#include "enemies/SimpleGreenSoldier.h"

/**
 * Constructor.
 * @param params name and position of the enemy to create
 */
SimpleGreenSoldier::SimpleGreenSoldier(const ConstructionParameters &params):
  Enemy(params) {

}

/**
 * Destructor.
 */
SimpleGreenSoldier::~SimpleGreenSoldier(void) {

}

/**
 * Initializes the enemy.
 */
void SimpleGreenSoldier::initialize(void) {

  // attack/defense properties
  set_damage(1);
  set_life(2);

  // movement
  // TODO set_movement(new SimpleGreenSoldierMovement());

  // sprite
  create_sprite("enemies/simple_green_soldier");
}
