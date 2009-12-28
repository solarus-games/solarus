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
#include "entities/Arrow.h"
#include "entities/Hero.h"
#include "entities/Enemy.h"
#include "entities/MapEntities.h"
#include "movements/PathMovement.h"
#include "movements/FollowMovement.h"
#include "Sprite.h"
#include "Map.h"
#include "lowlevel/System.h"

/**
 * Creates an arrow.
 * @param hero the hero
 */
Arrow::Arrow(Hero *hero):
  hero(hero) {

  // initialize the entity
  int direction = hero->get_animation_direction();
  set_layer(hero->get_layer());
  create_sprite("entities/arrow");
  get_sprite()->set_current_direction(direction);
  set_bounding_box_from_sprite();

  int hero_x = hero->get_top_left_x();
  int hero_y = hero->get_top_left_y();
  switch (hero->get_animation_direction()) {

    case 0:
      set_xy(hero_x + 24, hero_y + 8);
      break;

    case 1:
      set_xy(hero_x + 8, hero_y - 8);
      break;

    case 2:
      set_xy(hero_x - 8, hero_y + 8);
      break;

    case 3:
      set_xy(hero_x + 8, hero_y + 24);
      break;

  }

  std::string path = " ";
  path[0] = '0' + (direction * 2);
  Movement *movement = new PathMovement(path, 16, true, true, false);
  set_movement(movement);

  disappear_date = System::now() + 1500;
  stop_now = false;
  entity_reached = NULL;
}

/**
 * Destructor.
 */
Arrow::~Arrow(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Arrow::get_type() {
  return ARROW;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool Arrow::can_be_obstacle(void) {
  return false; 
}

/**
 * Returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool Arrow::can_detect_entities(void) {
  return false;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool Arrow::can_be_displayed(void) {
  return true; 
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Arrow::is_displayed_in_y_order(void) {
  return false;
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Arrow::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool Arrow::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return false;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool Arrow::is_water_obstacle(void) {
  return false;
}

/**
 * Returns whether a hole is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Arrow::is_hole_obstacle(void) {
  return false;
}

/**
 * Returns whether a ladder is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the ladders are currently an obstacle for this entity
 */
bool Arrow::is_ladder_obstacle(void) {
  return false;
}

/**
 * Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal switch block raised
 * @return false 
 */
bool Arrow::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  // arrows can traverse the crystal switch blocks
  return false;
}

/**
 * Returns whether a crystal switch is currently considered as an obstacle for this entity.
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool Arrow::is_crystal_switch_obstacle(CrystalSwitch *crystal_switch) {
  return false;
}

/**
 * Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * @param npc a non-playing character
 * @return true if the NPC is currently an obstacle for this entity
 */
bool Arrow::is_npc_obstacle(InteractiveEntity *npc) {
  return false;
}

/**
 * Returns whether a jump sensor is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool Arrow::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return false;
}

/**
 * Updates this entity.
 */
void Arrow::update(void) {

  MapEntity::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();
  if (stop_now) {
    clear_movement();

    if (entity_reached != NULL) {

      Rectangle dxy(get_x() - entity_reached->get_x(), get_y() - entity_reached->get_y());
      set_movement(new FollowMovement(entity_reached, dxy.get_x(), dxy.get_y(), false));
      disappear_date = now + 1500;
      stop_now = false;
    }
  }

  if (now >= disappear_date) {
    map->get_entities()->remove_entity(this);
  }
  else if (is_stopped()) {

    if (entity_reached == NULL) {
      if (disappear_date > now + 500) {
        // an obstacle was reached
        disappear_date = now + 500;
      }
    }
    else {
      // the entity reached disappeared
      disappear_date = now;
    }
  }
}

/**
 * This function is called by the map when the game is suspended or resumed.
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
 * Stops the arrow movement.
 */
void Arrow::stop(void) {
  stop_now = true;
}

/**
 * Returns whether the arrow is stopped.
 */
bool Arrow::is_stopped(void) {
  return get_movement() == NULL || get_movement()->is_finished();
}

/**
 * Returns whether the arrow is currently shot.
 * @return true if the arrow was shot and has not reached a target yet
 */
bool Arrow::is_shot(void) {
  return !is_stopped() && entity_reached == NULL;
}

/**
 * Stops the arrow movement and attaches the arrow to an entity that was just reached.
 * @param entity_reached the entity that was reached
 */
void Arrow::attach_to(MapEntity *entity_reached) {

  if (this->entity_reached != NULL) {
    DIE("This arrow is already attached to an entity");
  }

  this->entity_reached = entity_reached;
  stop_now = true;
}

/**
 * This function is called when an enemy collides with the entity.
 * @param enemy the enemy
 */
void Arrow::notify_collision_with_enemy(Enemy *enemy) {

  if (!overlaps(hero) && is_shot()) {
    enemy->try_hurt(ATTACK_BOW, this, NULL);
  }
}

/**
 * Notifies this entity that it has just attacked an enemy
 * (even if this attack was not successful).
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack:
 * - a number greater than 0 represents the number of health points the enemy has just lost
 * - a value of 0 means that the attack was just ignored 
 * - a value of -1 means that the enemy was protected against the attack
 * - a value of -2 means that the attack immobilized the enemy
 * @param killed indicates that the attack has just killed the enemy
 */
void Arrow::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {

  if (result == -1) {
    stop();
  }
  else if (result != 0) {
    if (killed) {
      stop();
      disappear_date = System::now();
    }
    else {
      attach_to(victim);
    }
  }
}

