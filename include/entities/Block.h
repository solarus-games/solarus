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
#ifndef SOLARUS_BLOCK_H
#define SOLARUS_BLOCK_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * @brief A entity that the hero can push or pull.
 *
 * Some blocks can be moved only once or in only one direction.
 */
class Block: public Detector {

  private:

    int maximum_moves;          /**< indicates whether the block can be pushed
                                 * (0: none, 1: once: 2: infinite) */
    bool sound_played;          /**< true if the block sound was played while pulling it */
    uint32_t when_can_move;     /**< date when the hero can move the block again */

    Rectangle last_position;    /**< last position of the block before moving */
    Rectangle initial_position; /**< position of the block when created */
    int initial_maximum_moves;  /**< value of maximum_moves when the block was created */
    bool can_be_pushed;         /**< indicates that the hero can push this block */
    bool can_be_pulled;         /**< indicates that the hero can pull this block */

    static const uint32_t moving_delay = 500; /**< delay between two successive moves of a block */

  public:

    Block(const std::string& name, Layer layer, int x, int y, int direction,
        const SpriteAnimationSetId& sprite_name,
        bool can_be_pushed, bool can_be_pulled, int maximum_moves);
    ~Block();
    static CreationFunction parse;

    EntityType get_type();
    bool is_displayed_in_y_order();

    bool is_obstacle_for(MapEntity& other);
    bool is_hole_obstacle();
    bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    bool is_hero_obstacle(Hero& hero);
    bool is_enemy_obstacle(Enemy& enemy);
    bool is_destructible_item_obstacle(DestructibleItem& destructible_item);
    void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode);
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void action_key_pressed();
    bool moved_by_hero();
    void notify_position_changed();

    void update();

    void reset();
};

#endif

