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
#include "solarus/entities/Boomerang.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Enemy.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Crystal.h"
#include "solarus/entities/Npc.h"
#include "solarus/entities/MapEntities.h"
#include "solarus/movements/TargetMovement.h"
#include "solarus/movements/StraightMovement.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Sound.h"

namespace Solarus {

/**
 * \brief Creates a boomerang.
 * \param hero the hero
 * \param max_distance maximum distance of the movement in pixels
 * \param speed speed of the movement in pixels per second
 * \param angle the angle of the boomerang trajectory in radians
 * \param sprite_name animation set id representing the boomerang
 */
Boomerang::Boomerang(
    const std::shared_ptr<Hero>& hero,
    int max_distance,
    int speed,
    double angle,
    const std::string& sprite_name
):
  MapEntity("", 0, hero->get_layer(), Point(0, 0), Size(0, 0)),
  hero(hero),
  has_to_go_back(false),
  going_back(false),
  speed(speed) {

  // initialize the entity
  create_sprite(sprite_name);
  set_size(16, 16);
  set_origin(8, 8);

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

  std::shared_ptr<StraightMovement> movement =
      std::make_shared<StraightMovement>(false, false);
  movement->set_speed(speed);
  movement->set_angle(angle);
  movement->set_max_distance(max_distance);
  set_movement(movement);

  next_sound_date = System::now();
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Boomerang::get_type() const {
  return EntityType::BOOMERANG;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 * \return true if this type of entity can be obstacle for other entities
 */
bool Boomerang::can_be_obstacle() const {
  return false;
}

/**
 * \brief Returns whether entities of this type can be drawn.
 * \return true if this type of entity can be drawn
 */
bool Boomerang::can_be_drawn() const {
  return true;
}

/**
 * \brief Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is currently an obstacle for this entity
 */
bool Boomerang::is_teletransporter_obstacle(Teletransporter& /* teletransporter */) {
  return false;
}

/**
 * \copydoc MapEntity::is_stream_obstacle
 */
bool Boomerang::is_stream_obstacle(Stream& /* stream */) {
  return false;
}

/**
 * \brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * \param stairs an stairs entity
 * \return true if the stairs are currently an obstacle for this entity
 */
bool Boomerang::is_stairs_obstacle(Stairs& stairs) {
  return stairs.is_inside_floor() && get_layer() == LAYER_LOW;
}

/**
 * \brief Returns whether a low wall is currently considered as an obstacle
 * by this entity.
 * \return \c true if low walls are currently obstacle for this entity.
 */
bool Boomerang::is_low_wall_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a deep water tile is currently considered as an obstacle for this entity.
 * \return true if the deep water tiles are currently an obstacle for this entity
 */
bool Boomerang::is_deep_water_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a hole is currently considered as an obstacle for this entity.
 * \return true if the holes are currently an obstacle for this entity
 */
bool Boomerang::is_hole_obstacle() const {
  return false;
}

/**
 * \brief Returns whether lava is currently considered as an obstacle for this entity.
 * \return true if lava is currently an obstacle for this entity
 */
bool Boomerang::is_lava_obstacle() const {
  return false;
}

/**
 * \brief Returns whether prickles are currently considered as an obstacle for this entity.
 * \return true if prickles are currently an obstacle for this entity
 */
bool Boomerang::is_prickle_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a ladder is currently considered as an obstacle for this entity.
 * \return true if the ladders are currently an obstacle for this entity
 */
bool Boomerang::is_ladder_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a switch is currently considered as an obstacle by this entity.
 * \param sw a switch
 * \return true if the switch is currently an obstacle for this entity
 */
bool Boomerang::is_switch_obstacle(Switch& /* sw */) {
  return false;
}

/**
 * \brief Returns whether a raised crystal block is currently considered as an obstacle for this entity.
 * \param raised_block a crystal block raised
 * \return false
 */
bool Boomerang::is_raised_block_obstacle(CrystalBlock& /* raised_block */) {
  // the boomerang can traverse crystal blocks
  return false;
}

/**
 * \brief Returns whether a crystal is currently considered as an obstacle for this entity.
 * \param crystal a crystal
 * \return true if the crystal is currently an obstacle for this entity
 */
bool Boomerang::is_crystal_obstacle(Crystal& /* crystal */) {
  return false;
}

/**
 * \brief Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * \param npc a non-playing character
 * \return true if the NPC is currently an obstacle for this entity
 */
bool Boomerang::is_npc_obstacle(Npc& npc) {
  return npc.is_solid();
}

/**
 * \copydoc MapEntity::is_jumper_obstacle
 */
bool Boomerang::is_jumper_obstacle(Jumper& /* jumper */, const Rectangle& /* candidate_position */) {
  return false;
}

/**
 * \brief Returns whether the boomerang is going back towards the hero, i.e. if go_back() has been called.
 * \return true if the boomerang is going back
 */
bool Boomerang::is_going_back() const {
  return has_to_go_back || going_back;
}

/**
 * \brief Makes the boomerang go back towards the hero.
 */
void Boomerang::go_back() {

  Debug::check_assertion(!is_going_back(), "The boomerang is already going back");

  has_to_go_back = true;
}

/**
 * \brief Updates the boomerang.
 */
void Boomerang::update() {

  MapEntity::update();

  if (is_suspended()) {
    return;
  }

  uint32_t now = System::now();
  if (now >= next_sound_date) {
    Sound::play("boomerang");
    next_sound_date = now + 150;
  }

  if (!going_back && has_to_go_back) {
    going_back = true;
    clear_movement();
    set_movement(std::make_shared<TargetMovement>(hero, 0, 0, speed, true));
    get_entities().set_entity_layer(*this, hero->get_layer()); // because the hero's layer may have changed
  }
}

/**
 * \brief Notifies this entity that it has just failed to change its position
 * because of obstacles.
 */
void Boomerang::notify_obstacle_reached() {

  if (!is_going_back()) {

    if (!get_map().test_collision_with_border(
        get_movement()->get_last_collision_box_on_obstacle())) {
      // play a sound unless the obstacle is the map border
      Sound::play("sword_tapping");
    }
    go_back();
  }
}

/**
 * \brief This function is called when the movement of the entity is finished.
 */
void Boomerang::notify_movement_finished() {

  if (!is_going_back()) {
    // the maximum distance is reached
    go_back();
  }
  else {
    // the boomerang is back
    remove_from_map();
  }
}

/**
 * \brief This function is called when a switch detects a collision with this entity.
 * \param sw the switch
 * \param collision_mode the collision mode that detected the event
 */
void Boomerang::notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_OVERLAPPING) {

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
void Boomerang::notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_OVERLAPPING) {

    crystal.activate(*this);
    if (!is_going_back()) {
      go_back();
    }
  }
}

/**
 * \brief This function is called when an enemy collides with the entity.
 * \param enemy the enemy
 */
void Boomerang::notify_collision_with_enemy(Enemy& enemy) {

  if (!overlaps(*hero)) {
    enemy.try_hurt(EnemyAttack::BOOMERANG, *this, nullptr);
  }
}

/**
 * \copydoc MapEntity::notify_attacked_enemy
 */
void Boomerang::notify_attacked_enemy(
    EnemyAttack /* attack */,
    Enemy& /* victim */,
    const Sprite* /* victim_sprite */,
    EnemyReaction::Reaction& result,
    bool /* killed */) {

  if (result.type != EnemyReaction::ReactionType::IGNORED && !is_going_back()) {
    go_back();
  }
}

}

