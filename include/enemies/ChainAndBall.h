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
#ifndef SOLARUS_CHAIN_AND_BALL_H
#define SOLARUS_CHAIN_AND_BALL_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * @brief A ball and its chain, usually controlled by another enemy.
 *
 * The ball is controlled by an instance of CircleMovement and
 * the chain automatically fits the space between the enemy and the ball.
 * The chain and ball cannot be killed. They usually disappear when
 * the enemy is killed.
 */
class ChainAndBall: public Enemy {

  private:

    // center
    MapEntity *center_entity;          /**< the entity this ball is making circles around */
    Rectangle center_entity_dxy;       /**< center point of the circles, relative to the entity */

    // chain
    static const int nb_links = 10;    /**< number of links in the chain */
    Sprite* link_sprite;               /**< sprite of the links between the center and the ball */
    Rectangle link_xy[nb_links];       /**< current coordinates of each link displayed */

  protected:

    void initialize(void);

  public:

    ChainAndBall(const ConstructionParameters &params);
    ~ChainAndBall(void);

    void attach_to(MapEntity *entity, int x = 0, int y = 0,
	int radius = 48, int radius_speed = 50, int max_rotations = 4, uint32_t loop_delay = 2000);
    void update(void);
    void display_on_map(void);
    void notify_just_moved(void);
    bool is_displayed_in_y_order(void);
    void restart(void);
};

#endif

