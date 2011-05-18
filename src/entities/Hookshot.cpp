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
#include "entities/Hookshot.h"
#include "entities/Enemy.h"
#include "entities/Chest.h"
#include "entities/DestructibleItem.h"
#include "entities/Block.h"
#include "entities/CrystalSwitch.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "movements/PathMovement.h"
#include "movements/TargetMovement.h"
#include "entities/Hero.h"
#include "entities/Stairs.h"
#include "Map.h"

/**
 * @brief Creates a hookshot.
 * @param hero the hero
 */
Hookshot::Hookshot(Hero &hero):
  next_sound_date(System::now()),
  has_to_go_back(false),
  going_back(false),
  entity_reached(NULL),
  link_sprite("entities/hookshot") {

  // initialize the entity
  int direction = hero.get_animation_direction();
  set_layer(hero.get_layer());
  create_sprite("entities/hookshot", true);
  get_sprite().set_current_direction(direction);
  link_sprite.set_current_animation("link");

  set_size(16, 16);
  set_origin(8, 13);
  set_xy(hero.get_xy());

  std::string path = " ";
  path[0] = '0' + (direction * 2);
  Movement *movement = new PathMovement(path, 192, true, false, false);
  set_movement(movement);
}

/**
 * @brief Destructor.
 */
Hookshot::~Hookshot() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Hookshot::get_type() {
  return HOOKSHOT;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * @return true if this type of entity can be obstacle for other entities
 */
bool Hookshot::can_be_obstacle() {
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
bool Hookshot::can_detect_entities() {
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
bool Hookshot::can_be_displayed() {
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
bool Hookshot::is_displayed_in_y_order() {
  return true;
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Hookshot::is_teletransporter_obstacle(Teletransporter &teletransporter) {
  return false;
}

/**
 * @brief Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool Hookshot::is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt) {
  return false;
}

/**
 * @brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * @param stairs an stairs entity
 * @return true if the stairs are currently an obstacle for this entity
 */
bool Hookshot::is_stairs_obstacle(Stairs &stairs) {
  return stairs.is_inside_floor() && get_layer() == LAYER_LOW;
}

/**
 * @brief Returns whether a deep water tile is currently considered as an obstacle for this entity.
 * @return true if the deep water tiles are currently an obstacle for this entity
 */
bool Hookshot::is_deep_water_obstacle() {
  return false;
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle for this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Hookshot::is_hole_obstacle() {
  return false;
}

/**
 * @brief Returns whether lava is currently considered as an obstacle for this entity.
 * @return true if lava is currently an obstacle for this entity
 */
bool Hookshot::is_lava_obstacle() {
  return false;
}

/**
 * @brief Returns whether prickles are currently considered as an obstacle for this entity.
 * @return true if prickles are currently an obstacle for this entity
 */
bool Hookshot::is_prickle_obstacle() {
  return false;
}

/**
 * @brief Returns whether a ladder is currently considered as an obstacle for this entity.
 * @return true if the ladders are currently an obstacle for this entity
 */
bool Hookshot::is_ladder_obstacle() {
  return false;
}

/**
 * @brief Returns whether a crystal switch is currently considered as an obstacle for this entity.
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool Hookshot::is_crystal_switch_obstacle(CrystalSwitch &crystal_switch) {
  return false;
}

/**
 * @brief Returns whether a non-diagonal jump sensor is currently considered as an obstacle for this entity.
 * @param jump_sensor a non-diagonal jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool Hookshot::is_jump_sensor_obstacle(JumpSensor &jump_sensor) {
  return false;
}

/**
 * @brief Returns the point located just outside the hookshot's collision box,
 * in its current direction.
 */
const Rectangle Hookshot::get_facing_point() {

  Rectangle facing_point = get_center_point();

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
      Debug::die(StringConcat() << "Invalid direction for Hookshot::get_facing_point(): "
          << get_sprite().get_current_direction());
  }

  return facing_point;
}

/**
 * @brief Updates this entity.
 */
void Hookshot::update() {

  MapEntity::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();
  if (now >= next_sound_date) {
    Sound::play("hookshot");
    next_sound_date = now + 150;
  }

  if (!going_back && entity_reached == NULL) {

    if (has_to_go_back) {
      going_back = true;
      Movement *movement = new TargetMovement(&get_hero(), 192);
      clear_movement();
      set_movement(movement);
    }
    else if (get_distance(get_hero()) >= 120) {
      go_back();
    }
  }
  else if (get_distance(get_hero()) == 0 ||
      (get_movement() != NULL && get_movement()->is_finished())) {
    remove_from_map();
    get_hero().start_free();
  }
}

/**
 * @brief Displays the entity on the map.
 */
void Hookshot::display_on_map() {

  static const int nb_links = 7;
  static const Rectangle dxy[] = {
    Rectangle(16, -5),
    Rectangle(0, -13),
    Rectangle(-16, -5),
    Rectangle(0, 7)
  };

  MapEntity::display_on_map();

  if (is_visible()) {

    // also display the links
    int direction = get_sprite().get_current_direction();
    int x1 = get_hero().get_x() + dxy[direction].get_x();
    int y1 = get_hero().get_y() + dxy[direction].get_y();
    int x2 = get_x();
    int y2 = get_y() - 5;

    Rectangle link_xy;
    for (int i = 0; i < nb_links; i++) {
      link_xy.set_x(x1 + (x2 - x1) * i / nb_links);
      link_xy.set_y(y1 + (y2 - y1) * i / nb_links);
      get_map().display_sprite(link_sprite, link_xy);
    }
  }
}

/**
 * @brief Returns whether the hookshot is currently flying.
 * @return true if the hookshot was shot, is not going back and has not reached any target yet
 */
bool Hookshot::is_flying() {
  return !is_going_back() && entity_reached == NULL;
}

/**
 * @brief Returns whether the hookshot is going back towards the hero, i.e. if go_back() has been called.
 * @return true if the hookshot is going back
 */
bool Hookshot::is_going_back() {
  return has_to_go_back || going_back;
}

/**
 * @brief Makes the hookshot go back towards the hero.
 */
void Hookshot::go_back() {

  Debug::check_assertion(!is_going_back(), "The hookshot is already going back");

  has_to_go_back = true;
}

/**
 * @brief Attachs the hookshot to an entity and makes the hero move towards this entity.
 * @param entity_reached the entity to attach the hookshot to
 */
void Hookshot::attach_to(MapEntity& entity_reached) {

  Debug::check_assertion(this->entity_reached == NULL,
      "The hookshot is already attached to an entity");

  this->entity_reached = &entity_reached;
  clear_movement();
  int direction = get_sprite().get_current_direction();
  std::string path = " ";
  path[0] = '0' + (direction * 2);
  get_hero().set_movement(new PathMovement(path, 192, true, false, false));
}

/**
 * @brief Notifies this entity that it has just tried to change its position.
 * @param success true if the position has actually just changed
 */
void Hookshot::notify_movement_tried(bool success) {

  if (!success && is_flying()) {
    if (!get_map().test_collision_with_border(get_movement()->get_last_collision_box_on_obstacle())) {
      // play a sound unless we are on the map border
      Sound::play("sword_tapping");
    }
    go_back();
  }
}

/**
 * @brief This function is called when an enemy collides with this entity.
 * @param enemy the enemy
 */
void Hookshot::notify_collision_with_enemy(Enemy &enemy) {

  if (!overlaps(get_hero())) {
    enemy.try_hurt(ATTACK_HOOKSHOT, *this, NULL);
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
void Hookshot::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {

  if (result.type != EnemyReaction::IGNORED && !is_going_back()) {
    go_back();
  }
}

/**
 * @brief This function is called when a chest detects a collision with this entity.
 * @param chest the chest
 */
void Hookshot::notify_collision_with_chest(Chest& chest) {

  if (is_flying()) {
    attach_to(chest);
  }
}

/**
 * @brief This function is called when a destructible item detects a non-pixel precise collision with this entity.
 * @param destructible_item the destructible item
 * @param collision_mode the collision mode that detected the event
 */
void Hookshot::notify_collision_with_destructible_item(DestructibleItem& destructible_item, CollisionMode collision_mode) {

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
 * @brief This function is called when a block detects a collision with this entity.
 * @param block the block
 */
void Hookshot::notify_collision_with_block(Block& block) {

  if (is_flying()) {
    attach_to(block);
  }
}

/**
 * @brief This function is called when a crystal switch detects a collision with this entity.
 * @param crystal_switch the crystal switch
 * @param collision_mode the collision mode that detected the event
 */
void Hookshot::notify_collision_with_crystal_switch(CrystalSwitch& crystal_switch, CollisionMode collision_mode) {

  if (is_flying()) {

    crystal_switch.activate(*this);
    if (!is_going_back()) {
      go_back();
    }
  }
}
