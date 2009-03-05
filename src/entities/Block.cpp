#include "entities/Block.h"
#include "entities/Hero.h"
#include "movements/PathMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "ResourceManager.h"
#include "KeysEffect.h"
#include "Sound.h"
#include "Sprite.h"

/**
 * Creates a block.
 * @param name name identifying this block
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param subtype the subtype of block
 * @param skin the skin (an animation name for the block sprite)
 * @param maximum_moves indicates how many times the block can
 * be moved (0: none, 1: once: 2: infinite)
 */
Block::Block(string name, Layer layer, int x, int y,
	     Subtype subtype, string skin, int maximum_moves):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  subtype(subtype), maximum_moves(maximum_moves), pulled(false) {

  create_sprite("entities/block");
  set_origin(8, 13);
  if (subtype == STATUE) {
    get_sprite()->set_current_animation("statue");
  }
  else {
    get_sprite()->set_current_animation(skin);
  }

  initial_position.x = last_position.x;
  initial_position.y = last_position.y;
}

/**
 * Destructor.
 */
Block::~Block() {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType Block::get_type(void) {
  return BLOCK;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool Block::is_obstacle_for(MapEntity *other) {
  return true;
}

/**
 * This function is called by the engine when there is a collision with another entity.
 * This is a redefinition of Detector::collision().
 * If the entity is the hero and this block can be pulled, we show the grab icon.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Block::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero()) {

    Hero *hero = zsdx->game->get_hero();
    KeysEffect *keys_effect = zsdx->game->get_keys_effect();

    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
	&& hero->get_state() == Hero::FREE) {

      // we show the action icon
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_GRAB);
    }
  }
}

/**
 * This function is called when the player pushes this block.
 * @param hero the hero
 */
void Block::pushed_by_hero(void) {

  if (movement == NULL && maximum_moves != 0) {

    Hero *hero = zsdx->game->get_hero();
    string path = "  ";
    path[0] = path[1] = '0' + hero->get_animation_direction() * 2;

    ResourceManager::get_sound("hero_pushes")->play();
    clear_movement();
    set_movement(new PathMovement(map, path, 12, false, true));
  }
}

/**
 * This function is called when the player tries to pull this detector.
 * @return true if the block was pulled successfully
 */
bool Block::pulled_by_hero(void) {

  if (movement != NULL || subtype != STATUE || pulled || maximum_moves == 0) {
    return false;
  }

  pulled = true;

  return true;
}

/**
 * Updates the entity.
 */
void Block::update(void) {

  Detector::update();

  Hero *hero = zsdx->game->get_hero();

  if (movement != NULL && ((PathMovement*) movement)->is_finished()) {

    // the block pushed has just finished moving
 
    clear_movement();

    if (get_x() != last_position.x || get_y() != last_position.y) {
      hero->stop_pushing_entity();
      last_position.x = get_x();
      last_position.y = get_y();

      if (maximum_moves == 1) {
	maximum_moves = 0; // cannot move any more
      }
    }
  }
  else if (pulled) {
    // the block is being pulled

    if (hero->get_state() == Hero::PULLING) {

      switch (hero->get_animation_direction()) {

      case 0:
	set_x(hero->get_x() + 16);
	break;

      case 1:
	set_y(hero->get_y() - 16);
	break;

      case 2:
	set_x(hero->get_x() - 16);
	break;

      case 3:
	set_y(hero->get_y() + 16);
	break;
      }

      // TODO if no collision,
      //      ResourceManager::get_sound("hero_pushes")->play();

    }
    else {
      pulled = false;
    }
  }
}

/**
 * Displays the entity again if it is a statue whose y position is greater than the hero's y position.
 */
void Block::display_on_map_above_hero(void) {

  Hero *hero = zsdx->game->get_hero();
  if (subtype == STATUE && get_y() > hero->get_y() + 8) {
    MapEntity::display_on_map();
  }
}
