/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "entities/Block.h"
#include "entities/Hero.h"
#include "entities/Switch.h"
#include "movements/FollowMovement.h"
#include "lua/MapScript.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/Sound.h"

/**
 * @brief Creates a block.
 * @param name name identifying this block
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param direction the only direction where the block can be pushed
 * or -1 to allow it to be pushed in any direction
 * @param sprite_name animation set id of the sprite for this block
 * @param can_be_pushed true to allow the hero to push this block
 * @param can_be_pulled true to allow the hero to pull this block
 * @param maximum_moves indicates how many times the block can
 * be moved (0: none, 1: once: 2: infinite)
 */
Block::Block(const std::string& name, Layer layer, int x, int y, int direction,
    const SpriteAnimationSetId& sprite_name,
    bool can_be_pushed, bool can_be_pulled, int maximum_moves):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  maximum_moves(maximum_moves),
  sound_played(false),
  when_can_move(System::now()),
  last_position(x, y),
  initial_position(x, y),
  initial_maximum_moves(maximum_moves),
  can_be_pushed(can_be_pushed),
  can_be_pulled(can_be_pulled) {

  set_origin(8, 13);
  set_direction(direction);
  create_sprite(sprite_name);
}

/**
 * @brief Destructor.
 */
Block::~Block() {

}

/**
 * @brief Creates an instance from an input stream.
 *
 * The input stream must respect the syntax of this entity type.
 *
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity* Block::parse(Game &game, std::istream &is, Layer layer, int x, int y) {

  int direction, maximum_moves, can_be_pushed, can_be_pulled;
  std::string name, sprite_name;

  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, sprite_name);
  FileTools::read(is, can_be_pushed);
  FileTools::read(is, can_be_pulled);
  FileTools::read(is, maximum_moves);

  return new Block(name, Layer(layer), x, y, direction, sprite_name,
      bool(can_be_pushed), bool(can_be_pulled), maximum_moves);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Block::get_type() {
  return BLOCK;
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether an entity of this type should be displayed above
 * the hero and other entities when it is in front of them.
 *
 * @return true if this entity is displayed at the same level as the hero
 */
bool Block::is_displayed_in_y_order() {
  return get_sprite().get_size().get_height() > 16;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool Block::is_obstacle_for(MapEntity& other) {

  return other.is_block_obstacle(*this);
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle for this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Block::is_hole_obstacle() {
  return false;
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Block::is_teletransporter_obstacle(Teletransporter& teletransporter) {
  // necessary to push a block into a hole having a teletransporter
  return false;
}

/**
 * @brief Returns whether the hero is currently considered as an obstacle by this entity.
 * @param hero the hero
 * @return true if the hero is an obstacle for this entity.
 */
bool Block::is_hero_obstacle(Hero& hero) {
  return get_movement() == NULL;
}

/**
 * @brief Returns whether an enemy is currently considered as an obstacle by this entity.
 * @param enemy an enemy
 * @return true if this enemy is currently considered as an obstacle by this entity.
 */
bool Block::is_enemy_obstacle(Enemy& enemy) {
  return true;
}

/**
 * @brief Returns whether a destructible item is currently considered as an obstacle by this entity.
 * @param destructible_item a destructible item
 * @return true if the destructible item is currently an obstacle by this entity
 */
bool Block::is_destructible_item_obstacle(DestructibleItem& destructible_item) {
  return true;
}

/**
 * @brief This function is called by the engine when there is a collision with another entity.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Block::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_block(*this);
}

/**
 * @brief This function is called when a switch detects a collision with this entity.
 * @param sw the switch
 * @param collision_mode the collision mode that detected the event
 */
void Block::notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) {

  sw.try_activate(*this);
}

/**
 * @brief Notifies this detector that the player is interacting by pressing the action key.
 *
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 */
void Block::action_key_pressed() {

  if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_GRAB) {
    get_hero().start_grabbing();
  }
}
 
/**
 * @brief This function is called when the player tries to push or pull this block.
 * @return true if the player is allowed to move this block
 */
bool Block::moved_by_hero() {

  Hero& hero = get_hero();
  bool pulling = hero.is_grabbing_or_pulling();
  int allowed_direction = get_direction();
  int hero_direction = hero.get_animation_direction();
  if (pulling) {
    // the movement direction is backwards
    hero_direction = (hero_direction + 2) % 4;
  }

  if (get_movement() != NULL                // the block is already moving
      || maximum_moves == 0                 // the block cannot move anymore
      || System::now() < when_can_move      // the block cannot move for a while
      || (pulling && !can_be_pulled)        // the hero tries to pull a block that cannot be pulled
      || (!pulling && !can_be_pushed)       // the hero tries to push a block that cannot be pushed
      || (allowed_direction != -1 && hero_direction != allowed_direction)) { // incorrect direction
    return false;
  }

  int dx = get_x() - hero.get_x();
  int dy = get_y() - hero.get_y();

  set_movement(new FollowMovement(&hero, dx, dy, false));
  sound_played = false;

  return true;
}

/**
 * @brief Updates the entity.
 */
void Block::update() {

  Detector::update();

  Hero& hero = get_hero();

  if (get_movement() != NULL) {
    // the block is being pushed or pulled by the hero

    // determine whether the movement is finished
    bool finished = false;

    if (get_movement()->is_finished()) {
      // the block was just stopped by an obstacle: notify the hero
      hero.notify_grabbed_entity_collision();
      finished = true;
    }
    else if (!hero.is_moving_grabbed_entity()) {
      // the hero stopped the movement, either because the 16 pixels were
      // covered or because the hero met an obstacle
      finished = true;
    }

    if (finished) {
      // the movement is finished (note that the block may have not moved)
      clear_movement();
      when_can_move = System::now() + moving_delay;

      // see if the block has moved
      if (get_x() != last_position.get_x() || get_y() != last_position.get_y()) {

        // the block has moved
        last_position.set_xy(get_x(), get_y()); // save the new position for next time

        if (maximum_moves == 1) { // if the block could be moved only once,
          maximum_moves = 0;      // then it cannot move anymore
        }
      }

      // notify the script
      get_map_script().event_block_moved(get_name());
    }
  }
  get_map().check_collision_with_detectors(*this);
}

/**
 * @brief Notifies the block that it has just moved.
 */
void Block::notify_position_changed() {

  // now we know that the block moves at least of 1 pixel:
  // we can play the sound
  if (!sound_played) {
    Sound::play("hero_pushes");
    sound_played = true;
  }
}

/**
 * @brief Resets the block at its initial position.
 */
void Block::reset() {

  if (get_movement() != NULL) {
    // the block was being pushed or pulled by the hero
    clear_movement();
    when_can_move = System::now() + moving_delay;
  }

  set_xy(initial_position);
  last_position.set_xy(initial_position);
  this->maximum_moves = initial_maximum_moves;
}

