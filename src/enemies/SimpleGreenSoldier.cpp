#include "enemies/SimpleGreenSoldier.h"
#include "movements/MovementWithCollision.h"
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
  set_damage(1);
  set_life(2);

  // movement
  set_movement(new MovementWithCollision(map));

  // sprite
  create_sprite("enemies/simple_green_soldier");
  set_size(16, 24);
  set_origin(8, 21);

  walk(direction);
}

/**
 * Updates the enemy.
 */
void SimpleGreenSoldier::update(void) {

  Enemy::update();

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
    std::cout << "sprite dir " << sprite->get_current_direction() << std::endl;
    if (animation == "stopped_watching_left") {
      walk((direction + 1) % 4);
    }
    else if (animation == "stopped_watching_right") {
      walk((direction + 3) % 4);
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
  get_movement()->set_speed(6);
  get_movement()->set_direction(direction * 90);
}
