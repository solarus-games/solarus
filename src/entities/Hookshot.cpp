/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Destructible.h"
#include "entities/Block.h"
#include "entities/Switch.h"
#include "entities/Crystal.h"
#include "entities/Hero.h"
#include "lowlevel/Debug.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "movements/PathMovement.h"
#include "movements/TargetMovement.h"
#include "entities/Hero.h"
#include "entities/Stairs.h"
#include "Map.h"

namespace solarus {

/**
 * \brief Creates a hookshot.
 * \param hero the hero
 */
Hookshot::Hookshot(Hero& hero):
  MapEntity("", 0, hero.get_layer(), 0, 0, 0, 0),
  next_sound_date(System::now()),
  has_to_go_back(false),
  going_back(false),
  entity_reached(NULL),
  link_sprite("entities/hookshot") {

  // initialize the entity
  int direction = hero.get_animation_direction();
  create_sprite("entities/hookshot", true);
  get_sprite().set_current_direction(direction);
  link_sprite.set_current_animation("link");

  set_size(16, 16);
  set_origin(8, 13);
  set_drawn_in_y_order(true);
  set_xy(hero.get_xy());

  std::string path = " ";
  path[0] = '0' + (direction * 2);
  Movement *movement = new PathMovement(path, 192, true, false, false);
  set_movement(movement);
}

/**
 * \brief Destructor.
 */
Hookshot::~Hookshot() {

}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Hookshot::get_type() const {
  return ENTITY_HOOKSHOT;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * \return true if this type of entity can be obstacle for other entities
 */
bool Hookshot::can_be_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is currently an obstacle for this entity
 */
bool Hookshot::is_teletransporter_obstacle(Teletransporter& teletransporter) {
  return false;
}

/**
 * \brief Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * \param conveyor_belt a conveyor belt
 * \return true if the conveyor belt is currently an obstacle for this entity
 */
bool Hookshot::is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt) {
  return false;
}

/**
 * \brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * \param stairs an stairs entity
 * \return true if the stairs are currently an obstacle for this entity
 */
bool Hookshot::is_stairs_obstacle(Stairs& stairs) {
  return stairs.is_inside_floor() && get_layer() == LAYER_LOW;
}

/**
 * \brief Returns whether a deep water tile is currently considered as an obstacle for this entity.
 * \return true if the deep water tiles are currently an obstacle for this entity
 */
bool Hookshot::is_deep_water_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a hole is currently considered as an obstacle for this entity.
 * \return true if the holes are currently an obstacle for this entity
 */
bool Hookshot::is_hole_obstacle() const {
  return false;
}

/**
 * \brief Returns whether lava is currently considered as an obstacle for this entity.
 * \return true if lava is currently an obstacle for this entity
 */
bool Hookshot::is_lava_obstacle() const {
  return false;
}

/**
 * \brief Returns whether prickles are currently considered as an obstacle for this entity.
 * \return true if prickles are currently an obstacle for this entity
 */
bool Hookshot::is_prickle_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a ladder is currently considered as an obstacle for this entity.
 * \return true if the ladders are currently an obstacle for this entity
 */
bool Hookshot::is_ladder_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a switch is currently considered as an obstacle by this entity.
 * \param sw a switch
 * \return true if the switch is currently an obstacle for this entity
 */
bool Hookshot::is_switch_obstacle(Switch& sw) {
  return false;
}

/**
 * \brief Returns whether a crystal is currently considered as an obstacle for this entity.
 * \param crystal a crystal
 * \return true if the crystal is currently an obstacle for this entity
 */
bool Hookshot::is_crystal_obstacle(Crystal& crystal) {
  return false;
}

/**
 * \brief Returns whether a non-diagonal jumper is currently considered as an obstacle for this entity.
 * \param jumper a non-diagonal jumper
 * \return true if the jumper is currently an obstacle for this entity
 */
bool Hookshot::is_jumper_obstacle(Jumper& jumper) {
  return false;
}

/**
 * \brief Returns the point located just outside the hookshot's collision box,
 * in its current direction.
 */
const Rectangle Hookshot::get_facing_point() const {

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
      Debug::die("Invalid direction for Hookshot::get_facing_point()");
  }

  return facing_point;
}

/**
 * \brief Updates this entity.
 */
void Hookshot::update() {

  MapEntity::update();

  if (is_suspended()) {
    return;
  }

  uint32_t now = System::now();
  if (now >= next_sound_date) {
    Sound::play("hookshot");
    next_sound_date = now + 150;
  }

  if (entity_reached == NULL) {
    if (!going_back) {

      if (has_to_go_back) {
        going_back = true;
        Movement* movement = new TargetMovement(&get_hero(), 0, 0, 192, true);
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
      get_hero().start_state_from_ground();
    }
  }
}

/**
 * \brief Draws the entity on the map.
 */
void Hookshot::draw_on_map() {

  static const int nb_links = 7;
  static const Rectangle dxy[] = {
    Rectangle(16, -5),
    Rectangle(0, -13),
    Rectangle(-16, -5),
    Rectangle(0, 7)
  };

  if (!is_drawn()) {
    return;
  }

  MapEntity::draw_on_map();

  // also draw the links
  int direction = get_sprite().get_current_direction();
  int x1 = get_hero().get_x() + dxy[direction].get_x();
  int y1 = get_hero().get_y() + dxy[direction].get_y();
  int x2 = get_x();
  int y2 = get_y() - 5;

  Rectangle link_xy;
  for (int i = 0; i < nb_links; i++) {
    link_xy.set_x(x1 + (x2 - x1) * i / nb_links);
    link_xy.set_y(y1 + (y2 - y1) * i / nb_links);
    get_map().draw_sprite(link_sprite, link_xy);
  }
}

/**
 * \brief Returns whether the hookshot is currently flying.
 * \return true if the hookshot was shot, is not going back and has not reached any target yet
 */
bool Hookshot::is_flying() const {
  return !is_going_back() && entity_reached == NULL;
}

/**
 * \brief Returns whether the hookshot is going back towards the hero, i.e. if go_back() has been called.
 * \return true if the hookshot is going back
 */
bool Hookshot::is_going_back() const {
  return has_to_go_back || going_back;
}

/**
 * \brief Makes the hookshot go back towards the hero.
 */
void Hookshot::go_back() {

  Debug::check_assertion(!is_going_back(), "The hookshot is already going back");

  has_to_go_back = true;
}

/**
 * \brief Attachs the hookshot to an entity and makes the hero move towards this entity.
 * \param entity_reached the entity to attach the hookshot to
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
 * \brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 */
void Hookshot::notify_obstacle_reached() {

  if (is_flying()) {
    if (!get_map().test_collision_with_border(
        get_movement()->get_last_collision_box_on_obstacle())) {
      // play a sound unless the obstacle is the map border
      Sound::play("sword_tapping");
    }
    go_back();
  }
}

/**
 * \brief This function is called when an enemy's sprite collides with a sprite of this entity.
 * \param enemy the enemy
 * \param enemy_sprite the enemy's sprite that overlaps the hero
 * \param this_sprite the arrow sprite
 */
void Hookshot::notify_collision_with_enemy(Enemy& enemy, Sprite& enemy_sprite, Sprite& this_sprite) {

  if (!overlaps(get_hero())) {
    enemy.try_hurt(ATTACK_HOOKSHOT, *this, &enemy_sprite);
  }
}

/**
 * \copydoc MapEntity::notify_attacked_enemy
 */
void Hookshot::notify_attacked_enemy(
    EnemyAttack attack,
    Enemy& victim,
    const Sprite* victim_sprite,
    EnemyReaction::Reaction& result,
    bool killed) {

  if (result.type != EnemyReaction::IGNORED && !is_going_back()) {
    go_back();
  }
}

/**
 * \brief This function is called when a chest detects a collision with this entity.
 * \param chest the chest
 */
void Hookshot::notify_collision_with_chest(Chest& chest) {

  if (is_flying()) {
    attach_to(chest);
  }
}

/**
 * \brief This function is called when a destructible item detects a non-pixel precise collision with this entity.
 * \param destructible the destructible item
 * \param collision_mode the collision mode that detected the event
 */
void Hookshot::notify_collision_with_destructible(Destructible& destructible, CollisionMode collision_mode) {

  if (destructible.is_obstacle_for(*this) && is_flying()) {

    if (destructible.get_can_explode()) {
      destructible.explode();
      go_back();
    }
    else {
      attach_to(destructible);
    }
  }
}

/**
 * \brief This function is called when a block detects a collision with this entity.
 * \param block the block
 */
void Hookshot::notify_collision_with_block(Block& block) {

  if (is_flying()) {
    attach_to(block);
  }
}

/**
 * \brief This function is called when a switch detects a collision with this entity.
 * \param sw the switch
 * \param collision_mode the collision mode that detected the event
 */
void Hookshot::notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) {

  if (is_flying() && collision_mode == COLLISION_OVERLAPPING) {

    sw.try_activate();
    if (!is_going_back()) {
      go_back();
      Sound::play("sword_tapping");
    }
  }
}

/**
 * \brief This function is called when a crystal detects a collision with this entity.
 * \param crystal the crystal
 * \param collision_mode the collision mode that detected the event
 */
void Hookshot::notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode) {

  if (is_flying()) {

    crystal.activate(*this);
    if (!is_going_back()) {
      go_back();
    }
  }
}

}

