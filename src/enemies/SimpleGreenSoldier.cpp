#include "enemies/SimpleGreenSoldier.h"
#include "movements/StraightMovement.h"
#include "Random.h"
#include "Sprite.h"

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
  set_damage(2);
  set_life(2);
  minimum_shield_needed = 2;

  // sprite
  create_sprite("enemies/simple_green_soldier");
  set_size(16, 16);
  set_origin(8, 13);

  // movement
  set_movement(new StraightMovement(map, 0, 0, 0));
}

/**
 * This function is called when the enemy need to restart its movement
 * because something happened (for example the enemy has just been created,
 * or it was just hurt).
 */
void SimpleGreenSoldier::restart(void) {
  Enemy::restart();
  walk(Random::get_number(4));
}

/**
 * Updates the enemy.
 */
void SimpleGreenSoldier::update(void) {

  Enemy::update();

  if (!is_killed()) {
    Sprite *sprite = get_sprite();
    string animation = sprite->get_current_animation();
    if (get_movement()->is_stopped() && animation == "walking") {

      int rand = Random::get_number(2);

      if (rand == 0) {
	sprite->set_current_animation("stopped_watching_left");
      }
      else {
	sprite->set_current_animation("stopped_watching_right");
      }
    }

    if (sprite->is_over()) {

      int direction = sprite->get_current_direction();
      if (animation == "stopped_watching_left") {
	walk((direction + 1) % 4);
      }
      else if (animation == "stopped_watching_right") {
	walk((direction + 3) % 4);
      }
    }
  }
}

/**
 * Starts walking towards one of the four main directions.
 * @param direction a direction (0 to 3)
 */
void SimpleGreenSoldier::walk(int direction) {

  Sprite *sprite = get_sprite();
  sprite->set_current_animation("walking");
  sprite->set_current_direction(direction);

  StraightMovement *movement = (StraightMovement*) get_movement();
  int seconds = 2 + Random::get_number(3);
  movement->start(4, direction * 90, seconds * 1000);
}
