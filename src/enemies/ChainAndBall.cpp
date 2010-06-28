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
#include "enemies/ChainAndBall.h"
#include "movements/CircleMovement.h"
#include "Sprite.h"
#include "Map.h"

const int ChainAndBall::nb_links;

/**
 * @brief Constructor.
 * @param params name and position of the enemy to create
 */
ChainAndBall::ChainAndBall(const ConstructionParameters &params):
  Enemy(params), center_entity(NULL) {

}

/**
 * @brief Destructor.
 */
ChainAndBall::~ChainAndBall(void) {

}

/**
 * @brief Initializes this enemy.
 */
void ChainAndBall::initialize(void) {

  // attack/defense features
  set_damage(2);
  set_life(1);

  // sprites
  create_sprite("enemies/chain_and_ball");
  get_sprite()->set_current_animation("walking");
  link_sprite = new Sprite("enemies/chain_and_ball");
  link_sprite->set_current_animation("chain");

  // rectangle
  set_size(16, 16);
  set_origin(8, 8);

  // reactions to attacks
  set_no_attack_consequences();
}

/**
 * @brief Updates this enemy.
 */
void ChainAndBall::update(void) {
  Enemy::update();
}

/**
 * @brief Displays the chain and ball on the map.
 */
void ChainAndBall::display_on_map(void) {

  Enemy::display_on_map();

  if (is_visible()) {
    // also display the chain
    for (int i = 0; i < nb_links; i++) {
      map->display_sprite(link_sprite, link_xy[i].get_x(), link_xy[i].get_y());
    }
  }
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether the entity should be displayed above
 * the hero and other entities having this property when it is in front of them.
 *
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool ChainAndBall::is_displayed_in_y_order(void) {
  return false; // unlike usual enemies
}

/**
 * @brief Makes the chain and ball follow the specified entity.
 * @param entity the entity to follow
 * @param x x coordinate of where the chain movement should be centered on (relative to the entity followed)
 * @param y y coordinate of where the chain movement should be centered on (relative to the entity followed)
 * @param radius radius of the circles to make, in pixels (see CircleMovement::set_radius)
 * @param radius_speed speed of the radius variation in pixels per second (see CircleMovement::set_radius_speed)
 * @param max_rotations number of rotations to make (see CircleMovement::set_max_rotations)
 * @param loop_delay the movement will restart after this delay in milliseconds (see CircleMovement::set_loop)
 */
void ChainAndBall::attach_to(MapEntity *entity, int x, int y,
    int radius, int radius_speed, int max_rotations, uint32_t loop_delay) {

  clear_movement();

  CircleMovement *movement = new CircleMovement();
  movement->set_center(entity, x, y);
  movement->set_angle_speed(360);
  movement->set_radius_speed(radius_speed);
  movement->set_radius(radius);
  movement->set_max_rotations(max_rotations);
  movement->set_loop(loop_delay);
  movement->start();
  set_movement(movement);

  this->center_entity = entity;
  this->center_entity_dxy = Rectangle(x, y);
}

/**
 * @brief This function is called when the ball has just moved.
 *
 * The chain is then updated.
 */
void ChainAndBall::notify_just_moved(void) {

  Enemy::notify_just_moved();

  if (center_entity != NULL) {
    // recalculate the chain position
    
    int x1 = center_entity->get_x() + center_entity_dxy.get_x();
    int y1 = center_entity->get_y() + center_entity_dxy.get_y();
    int x2 = get_x();
    int y2 = get_y();


    for (int i = 0; i < nb_links; i++) {
      int x = x1 + (x2 - x1) * i / nb_links;
      int y = y1 + (y2 - y1) * i / nb_links;
      link_xy[i].set_xy(x, y);
    }
  }
}

