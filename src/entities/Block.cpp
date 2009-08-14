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
#include "FileTools.h"

/**
 * Creates a block.
 * @param name name identifying this block
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param direction the only direction where the block can be pushed
 * (only for a normal block) or -1 to allow it to be pushed in any direction
 * @param subtype the subtype of block
 * @param maximum_moves indicates how many times the block can
 * be moved (0: none, 1: once: 2: infinite)
 */
Block::Block(const std::string &name, Layer layer, int x, int y,
	     int direction, Subtype subtype, int maximum_moves):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  subtype(subtype), maximum_moves(maximum_moves), sound_played(false),
  when_can_move(SDL_GetTicks()) {

  set_origin(8, 13);
  if (subtype == STATUE) {

    if (direction != -1) {
      DIE("Cannot set only one direction for a statue");
    }
    create_sprite("entities/statue");
  }
  else {
    create_sprite("entities/block");
  }

  set_direction(direction);

  initial_position.x = last_position.x = x;
  initial_position.y = last_position.y = y;
  initial_maximum_moves = maximum_moves;
}

/**
 * Destructor.
 */
Block::~Block(void) {

}

/**
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * Block::parse(std::istream &is, Layer layer, int x, int y) {

  int direction, subtype, maximum_moves;
  std::string name;

  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, subtype);
  FileTools::read(is, maximum_moves);

  return new Block(name, Layer(layer), x, y, direction, Subtype(subtype), maximum_moves);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Block::get_type(void) {
  return BLOCK;
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities when it is in front of them.
 * @return true if this entity is displayed at the same level as the hero
 */
bool Block::is_displayed_in_y_order(void) {
  return subtype == STATUE;
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

    Hero *hero = (Hero*) entity_overlapping;
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
 * @return true if the player can be move this block
 */
bool Block::moved_by_hero(void) {

  Hero *hero = zsdx->game->get_hero();

  if (get_movement() != NULL || maximum_moves == 0 || SDL_GetTicks() < when_can_move ||
      (direction != -1 && hero->get_animation_direction() != direction)) {
    /* TODO remove
    if (maximum_moves == 0)
      std::cout << "not moving because cannot move any more\n";
    else if (get_movement() != NULL) 
      std::cout << "not moving because already moving\n";
    else if...
    */
    return false;
  }

  if (hero->is_grabbing_or_pulling() && subtype != STATUE) {
    return false;
  }

  int dx = get_x() - hero->get_x();
  int dy = get_y() - hero->get_y();

  set_movement(new FollowMovement(hero, dx, dy, true));
  sound_played = false;

  return true;
}

/**
 * Updates the entity.
 */
void Block::update(void) {

  Detector::update();

  Hero *hero = zsdx->game->get_hero();

  if (movement != NULL) {
    // the block is being pushed or pulled by the hero

    // determine whether the movement is finished
    bool finished = false;

    if (movement->is_finished()) {
      // the block was just stopped by an obstacle: notify the hero
      hero->grabbed_entity_collision();
      finished = true;
    }
    else if (!hero->is_moving_grabbed_entity()) {
      // the hero stopped the movement, either because the 16 pixels were
      // covered or because the hero met an obstacle
      finished = true;
    }

    if (finished) {
      // the movement is finished (note that the block may have not moved)
      clear_movement();
      when_can_move = SDL_GetTicks() + 500;

      // see if the block has moved
      if (get_x() != last_position.x || get_y() != last_position.y) {

	// the block has moved
	last_position.x = get_x(); // save the new position for next time
	last_position.y = get_y();

	if (maximum_moves == 1) { // if the block could be moved only once
	  maximum_moves = 0;      // it cannot move any more
	}
      }
    }
  }
}

/**
 * Notifies the block that it has just moved.
 */
void Block::just_moved(void) {

  // now we now that the block moves at least of 1 pixel:
  // we can play the sound
  if (!sound_played) {
    ResourceManager::get_sound("hero_pushes")->play();
    sound_played = true;
  }
}

/**
 * Resets the block at its initial position.
 */
void Block::reset(void) {
  set_xy(initial_position);
  this->maximum_moves = initial_maximum_moves;
}
