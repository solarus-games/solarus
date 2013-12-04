/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
 * \brief A entity that the hero can push or pull.
 *
 * Some blocks can be moved only once or in only one direction.
 */
class Block: public Detector {

  public:

    Block(const std::string& name,
        Layer layer,
        int x,
        int y,
        int direction,
        const std::string& sprite_name,
        bool can_be_pushed,
        bool can_be_pulled,
        int maximum_moves);
    ~Block();

    EntityType get_type() const;
    bool is_drawn_in_y_order() const;
    bool is_ground_observer() const;

    bool is_obstacle_for(const MapEntity& other) const;
    bool is_hole_obstacle() const;
    bool is_teletransporter_obstacle(
        const Teletransporter& teletransporter) const;
    bool is_hero_obstacle(const Hero& hero) const;
    bool is_enemy_obstacle(const Enemy& enemy) const;
    bool is_destructible_obstacle(const Destructible& destructible) const;
    void set_map(Map& map);
    void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode);
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void notify_action_command_pressed();
    bool start_movement_by_hero();
    void stop_movement_by_hero();
    void notify_moving_by(MapEntity& entity);
    void notify_moved_by(MapEntity& entity);
    void notify_position_changed();
    void notify_obstacle_reached();
    void notify_ground_below_changed();

    void reset();
    bool is_pushable() const;
    void set_pushable(bool pushable);
    bool is_pullable() const;
    void set_pullable(bool pullable);
    int get_maximum_moves() const;
    void set_maximum_moves(int maximum_moves);

    virtual const std::string& get_lua_type_name() const;

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

    void movement_by_hero_finished();

};

#endif

