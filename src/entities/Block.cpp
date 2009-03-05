#include "entities/Block.h"
#include "entities/Hero.h"
#include "movements/FollowMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
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
  subtype(subtype), maximum_moves(maximum_moves), sound_played(false) {

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

  if (other->is_hero() && get_movement() != NULL) {
    return false;
  }

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
 * This function is called when the player tries to push or pull this block.
 * @param hero the hero
 * @return true if the player can be move this block
 */
bool Block::moved_by_hero(void) {

  if (get_movement() != NULL || maximum_moves == 0) {
    return false;
  }

  Hero *hero = zsdx->game->get_hero();

  if (subtype != STATUE && hero->get_state() == Hero::PULLING) {
    return false;
  }

  int dx = get_x() - hero->get_x();
  int dy = get_y() - hero->get_y();

  set_movement(new FollowMovement(map, hero, dx, dy, true));
  sound_played = false; // TODO
  ResourceManager::get_sound("hero_pushes")->play();

  return true;
}

/**
 * Updates the entity.
 */
void Block::update(void) {

  Detector::update();

  Hero *hero = zsdx->game->get_hero();

  if (movement != NULL && ((FollowMovement*) movement)->is_finished()) {

    // the block moved was just stopped by an obstacle
    clear_movement();

    if (get_x() != last_position.x || get_y() != last_position.y) {
      hero->stop_moving_facing_entity();

      // TODO...
      last_position.x = get_x();
      last_position.y = get_y();

      if (maximum_moves == 1) {
	maximum_moves = 0; // cannot move any more
      }
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
