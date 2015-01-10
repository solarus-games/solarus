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
#include "solarus/entities/Explosion.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Crystal.h"
#include "solarus/entities/Sensor.h"
#include "solarus/entities/Enemy.h"
#include "solarus/Game.h"
#include "solarus/Sprite.h"
#include "solarus/SpriteAnimationSet.h"

namespace Solarus {

/**
 * \brief Creates an explosion.
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer layer of the explosion
 * \param xy coordinates of the center of the explosion
 * \param with_damages true to hurt the hero and the enemies
 */
Explosion::Explosion(const std::string& name, Layer layer,
    const Point& xy, bool with_damages):
  Detector(COLLISION_SPRITE | COLLISION_OVERLAPPING, name, layer, xy, Size(48, 48)) {

  // initialize the entity
  create_sprite("entities/explosion");

  set_optimization_distance(2000); // because of placing a bomb on a switch
  get_sprite().enable_pixel_collisions();
  if (with_damages) {
    set_size(48, 48);
    set_origin(24, 24);
  }
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Explosion::get_type() const {
  return EntityType::EXPLOSION;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * \return true if this type of entity can be obstacle for other entities
 */
bool Explosion::can_be_obstacle() const {
  return false;
}


/**
 * \brief Updates this entity.
 */
void Explosion::update() {

  Detector::update();

  if (get_sprite().is_animation_finished()) {
    remove_from_map();
  }
}

/**
 * \brief Notifies this entity that the frame of one of its sprites has just changed.
 * \param sprite the sprite
 * \param animation the current animation
 * \param frame the new frame
 */
void Explosion::notify_sprite_frame_changed(Sprite& /* sprite */, const std::string& /* animation */, int frame) {

  if (frame == 1) {
    // also detect non-pixel precise collisions
    check_collision_with_detectors();
  }
}

/**
 * \copydoc Detector::notify_collision(MapEntity&, Sprite&, Sprite&)
 */
void Explosion::notify_collision(
    MapEntity& other_entity,
    Sprite& /* this_sprite */,
    Sprite& other_sprite
) {
  other_entity.notify_collision_with_explosion(*this, other_sprite);
}

/**
 * \brief This function is called when a the sprite of a switch
 * detects a pixel-precise collision with a sprite of this entity.
 * \param sw the switch
 * \param sprite_overlapping the sprite of the current entity that collides with the switch
 */
void Explosion::notify_collision_with_switch(Switch& sw, Sprite& /* sprite_overlapping */) {

  sw.try_activate();
}

/**
 * \brief This function is called when a the sprite of a crystal
 * detects a pixel-precise collision with a sprite of this entity.
 * \param crystal the crystal
 * \param sprite_overlapping the sprite of the current entity that collides with the crystal
 */
void Explosion::notify_collision_with_crystal(Crystal& crystal, Sprite& /* sprite_overlapping */) {

  crystal.activate(*this);
}

/**
 * \brief This function is called when a sensor detects a collision with this entity.
 * \param sensor a sensor
 * \param collision_mode the collision mode that detected the collision
 */
void Explosion::notify_collision_with_sensor(Sensor& sensor, CollisionMode collision_mode) {

  if (collision_mode == COLLISION_OVERLAPPING) {
    sensor.notify_collision_with_explosion(*this, collision_mode);
  }
}

/**
 * \brief This function is called when an enemy's sprite collides with a sprite of this entity
 * \param enemy the enemy
 * \param enemy_sprite the enemy's sprite that overlaps a sprite of this entity
 * \param this_sprite this entity's sprite that overlaps the enemy's sprite
 */
void Explosion::notify_collision_with_enemy(Enemy& enemy, Sprite& enemy_sprite, Sprite& /* this_sprite */) {

  try_attack_enemy(enemy, enemy_sprite);
}

/**
 * \brief Attacks the specified enemy if possible.
 *
 * This function is called by this explosion when it detects an enemy, or by an enemy who detects this explosion.
 *
 * \param enemy the enemy to attack
 * \param enemy_sprite the enemy's sprite detected by the explosion
 */
void Explosion::try_attack_enemy(Enemy& enemy, Sprite& enemy_sprite) {

  // see if the enemy was already hurt by this explosion
  for (Enemy* victim: victims) {
    if (victim == &enemy) {
      return;
    }
  }

  enemy.try_hurt(EnemyAttack::EXPLOSION, *this, &enemy_sprite);
}

/**
 * \copydoc MapEntity::notify_attacked_enemy
 */
void Explosion::notify_attacked_enemy(
    EnemyAttack /* attack */,
    Enemy& victim,
    const Sprite* /* victim_sprite */,
    EnemyReaction::Reaction& result,
    bool /* killed */) {

  if (result.type != EnemyReaction::ReactionType::IGNORED) {
    victims.push_back(&victim);
  }
}

}

