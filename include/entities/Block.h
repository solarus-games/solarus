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
#ifndef ZSDX_BLOCK_H
#define ZSDX_BLOCK_H

#include "Common.h"
#include "Detector.h"

/**
 * A block or a statue that the hero can push.
 * If this a statue, it can also be pulled.
 */
class Block: public Detector {

  public:

    /**
     * The different kinds of blocks: normal block
     * or statue.
     */
    enum Subtype {
      NORMAL, /**< a usual block */
      STATUE, /**< a statue */
    };

  private:

    Rectangle last_position;    /**< last position of the block before moving */
    Subtype subtype;            /**< normal block or statue */
    int maximum_moves;          /**< indicates whether the block can be pushed
				 * (0: none, 1: once: 2: infinite) */
    bool sound_played;          /**< true if the block sound was played while pulling it */
    uint32_t when_can_move;     /**< date when the hero can move the block again */

    Rectangle initial_position; /**< position of the block when created */
    int initial_maximum_moves;  /**< value of maximum_moves when the block was created */

  public:

    Block(const std::string &name, Layer layer, int x, int y,
	int direction, Subtype subtype, int maximum_push);
    ~Block(void);
    static CreationFunction parse;

    EntityType get_type(void);
    bool is_displayed_in_y_order(void);

    bool is_obstacle_for(MapEntity *other);
    bool is_enemy_obstacle(Enemy *enemy);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    bool moved_by_hero(void);
    void just_moved(void);

    void update(void);

    void reset(void);
};

#endif

