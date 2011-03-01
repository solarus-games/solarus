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
#include "entities/Arrow.h"
#include "entities/Hero.h"
#include "entities/Enemy.h"
#include "entities/Stairs.h"
#include "entities/Switch.h"
#include "entities/CrystalSwitch.h"
#include "entities/DestructibleItem.h"
#include "movements/PathMovement.h"
#include "movements/FollowMovement.h"
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates an arrow.
 * @param hero the hero
 */
Arrow::Arrow(Hero &hero):
  hero(hero) {

  // initialize the entity
  int direction = hero.get_animation_direction();
  set_layer(hero.get_layer());
  create_sprite("entities/arrow", true);
  get_sprite().set_current_direction(direction);
  set_bounding_box_from_sprite();
  set_xy(hero.get_center_point());

  std::string path = " ";
  path[0] = '0' + (direction * 2);
  Movement *movement = new PathMovement(path, 192, true, false, false);
  set_movement(movement);

  disappear_date = System::now() + 1500;
  stop_now = false;
  entity_reached = NULL;
}

/**
 * @brief Destructor.
 */
Arrow::~Arrow() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Arrow::get_type() {
  return ARROW;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * @return true if this type of entity can be obstacle for other entities
 */
bool Arrow::can_be_obstacle() {
  return false; 
}

/**
 * @brief Returns whether entities of this type have detection capabilities.
 *
 * This function returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 *
 * @return true if this type of entity can detect other entities
 */
bool Arrow::can_detect_entities() {
  return false;
}

/**
 * @brief Returns whether entities of this type can be displayed.
 *
 * If yes, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 *
 * @return true if this type of entity can be displayed
 */
bool Arrow::can_be_displayed() {
  return true; 
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 *
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Arrow::is_displayed_in_y_order() {
  return true;
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Arrow::is_teletransporter_obstacle(Teletransporter &teletransporter) {
  return false;
}

/**
 * @brief Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool Arrow::is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt) {
  return false;
}

/**
 * @brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * @param stairs an stairs entity
 * @return true if the stairs are currently an obstacle for this entity
 */
bool Arrow::is_stairs_obstacle(Stairs &stairs) {
  return stairs.is_inside_floor() && get_layer() == LAYER_LOW;
}

/**
 * @brief Returns whether a deep water tile is currently considered as an obstacle for this entity.
 * @return true if the deep water tiles are currently an obstacle for this entity
 */
bool Arrow::is_deep_water_obstacle() {
  return false;
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle for this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Arrow::is_hole_obstacle() {
  return false;
}

/**
 * @brief Returns whether a ladder is currently considered as an obstacle for this entity.
 * @return true if the ladders are currently an obstacle for this entity
 */
bool Arrow::is_ladder_obstacle() {
  return false;
}

/**
 * @brief Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal switch block raised
 * @return false 
 */
bool Arrow::is_raised_block_obstacle(CrystalSwitchBlock &raised_block) {
  // arrows can traverse the crystal switch blocks
  return false;
}

/**
 * @brief Returns whether a crystal switch is currently considered as an obstacle for this entity.
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool Arrow::is_crystal_switch_obstacle(CrystalSwitch &crystal_switch) {
  return false;
}

/**
 * @brief Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * @param npc a non-playing character
 * @return true if the NPC is currently an obstacle for this entity
 */
bool Arrow::is_npc_obstacle(InteractiveEntity &npc) {
  return false;
}

/**
 * @brief Returns whether a non-diagonal jump sensor is currently considered as an obstacle for this entity.
 * @param jump_sensor a non-diagonal jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool Arrow::is_jump_sensor_obstacle(JumpSensor &jump_sensor) {
  return false;
}

/**
 * @brief Returns the point located just outside the arrow's collision box,
 * in its current direction.
 */
const Rectangle Arrow::get_facing_point() {

  Rectangle facing_point = get_xy();

  switch (get_sprite().get_current_direction()) {

    // right
    case 0:
      facing_point.add_x(8);
      break;

      // up
    case 1:
      facing_point.add_y(-9);
      break;

      // left
    case 2:
      facing_point.add_x(-9);
      break;

      // down
    case 3:
      facing_point.add_y(8);
      break;

    default:
      Debug::die(StringConcat() << "Invalid direction for Arrow::get_facing_point(): "
	  << get_sprite().get_current_direction());
  }

  return facing_point;
}

/**
 * @brief Updates this entity.
 */
void Arrow::update() {

  MapEntity::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();
    
  // stop the movement if necessary (i.e. stop() was called)
  if (stop_now) {
    clear_movement();
    stop_now = false;

    if (entity_reached != NULL) {
      // the arrow just hit an entity (typically an enemy) and this entity may have a movement
      Rectangle dxy(get_x() - entity_reached->get_x(), get_y() - entity_reached->get_y());
      set_movement(new FollowMovement(entity_reached, dxy.get_x(), dxy.get_y(), true));
    }
  }

  if (entity_reached != NULL) {
    
    // see if the entity reached is still valid
    if (is_stopped()) {
      // the arrow is stopped because the entity that was reached just disappeared
      disappear_date = now;
    }
    else if (entity_reached->get_type() == DESTRUCTIBLE_ITEM && !entity_reached->is_obstacle_for(*this)) {
      disappear_date = now;
    }
    else if (entity_reached->get_type() == ENEMY && ((Enemy*) entity_reached)->is_dying()) {
      // the enemy is dying
      disappear_date = now;
    }
  }

  // see if the arrow just hit a wall or an entity
  bool reached_obstacle = false;

  if (get_sprite().get_current_animation() != "reached_obstacle") {

    if (entity_reached != NULL) {
      // the arrow was just attached to an entity
      reached_obstacle = true;
    }
    else if (is_stopped()) {

      if (has_reached_map_border()) {
        // the map border was reached: destroy the arrow
	disappear_date = now;
      }
      else {
	// the arrow has just hit another obstacle
	reached_obstacle = true;
      }
    }
  }

  if (reached_obstacle) {
    // an obstacle or an entity was just reached
    disappear_date = now + 1500;
    get_sprite().set_current_animation("reached_obstacle");
    Sound::play("arrow_hit");

    if (entity_reached == NULL) {
      clear_movement();
    }
    get_map().check_collision_with_detectors(*this);
  }

  // destroy the arrow when disappear_date is reached
  if (now >= disappear_date) {
    remove_from_map();
  }
}

/**
 * @brief This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the entity, false to resume it
 */
void Arrow::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the movement

  if (!suspended) {
    // recalculate the timer
    disappear_date += System::now() - when_suspended;
  }
}

/**
 * @brief Stops the arrow movement.
 */
void Arrow::stop() {
  stop_now = true;
}

/**
 * @brief Returns whether the arrow is stopped.
 * @return true if the arrow is stopped
 */
bool Arrow::is_stopped() {
  return get_movement() == NULL || get_movement()->is_finished();
}

/**
 * @brief Returns whether the arrow is currently flying.
 * @return true if the arrow was shot and has not reached a target yet
 */
bool Arrow::is_flying() {
  return !is_stopped() && entity_reached == NULL;
}

/**
 * @brief Stops the arrow movement and attaches the arrow to an entity that was just reached.
 * @param entity_reached the entity that was reached
 */
void Arrow::attach_to(MapEntity &entity_reached) {

  Debug::check_assertion(this->entity_reached == NULL, "This arrow is already attached to an entity");

  this->entity_reached = &entity_reached;
  stop_now = true;
}

/**
 * @brief This function is called when a switch detects a collision with this entity.
 * @param sw the switch
 */
void Arrow::notify_collision_with_switch(Switch &sw) {

  if (is_stopped()) {
    sw.try_activate(*this);
  }
}

/**
 * @brief This function is called when a crystal switch detects a collision with this entity.
 * @param crystal_switch the crystal switch
 * @param collision_mode the collision mode that detected the event
 */
void Arrow::notify_collision_with_crystal_switch(CrystalSwitch &crystal_switch, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_RECTANGLE && is_flying()) {

    crystal_switch.activate(*this);
    attach_to(crystal_switch);
  }
}

/**
 * @brief This function is called when a destructible item detects a non-pixel perfect collision with this entity.
 * @param destructible_item the destructible item
 * @param collision_mode the collision mode that detected the event
 */
void Arrow::notify_collision_with_destructible_item(DestructibleItem &destructible_item, CollisionMode collision_mode) {

  if (destructible_item.is_obstacle_for(*this) && is_flying()) {

    if (destructible_item.can_explode()) {
      destructible_item.explode();
      remove_from_map();
    }
    else {
      attach_to(destructible_item);
    }
  }
}

/**
 * @brief This function is called when an enemy's sprite collides with a sprite of this entity.
 * @param enemy the enemy
 * @param enemy_sprite the enemy's sprite that overlaps the hero
 * @param this_sprite the arrow sprite
 */
void Arrow::notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite) {

  if (!overlaps(hero) && is_flying()) {
    enemy.try_hurt(ATTACK_ARROW, *this, NULL);
  }
}

/**
 * @brief Notifies this entity that it has just attacked an enemy.
 *
 * This function is called even if this attack was not successful.
 *
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack
 * @param killed indicates that the attack has just killed the enemy
 */
void Arrow::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {

  if (result.type == EnemyReaction::PROTECTED) {
    stop();
  }
  else if (result.type != EnemyReaction::IGNORED) {
    if (killed) {
      remove_from_map();
    }
    else {
      attach_to(victim);
    }
  }
}

/**
 * @brief Returns whether the arrow has just hit the map border.
 * @return true if the arrow has just hit the map border
 */
bool Arrow::has_reached_map_border() {

  if (get_sprite().get_current_animation() != "flying" || get_movement() == NULL) {
    return false;
  }

  return get_map().test_collision_with_border(get_movement()->get_last_collision_box_on_obstacle());
}

