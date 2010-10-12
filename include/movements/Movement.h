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
#ifndef SOLARUS_MOVEMENT_H
#define SOLARUS_MOVEMENT_H

#include "Common.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Abstract class for representing a movement.
 *
 * This is the parent class of all kinds of movement.
 * An instance of Movement can be applied to a map entity during the game,
 * or to any arbitrary coordinates.
 */
class Movement {

  private:

    // object to move
    MapEntity *entity;					/**< the entity controlled by this movement (if NULL, the movement
							 * is applied to the xy field below instead) */

    Rectangle xy;					/**< coordinates of the object controlled by this movement when it is not an entity */

    // suspended
    bool suspended;					/**< indicates whether the movement is suspended */
    uint32_t when_suspended;				/**< indicates when the movement was suspended */

    // obstacles (only when the movement is applied to an entity)
    Rectangle last_collision_box_on_obstacle;		/**< copy of the entity's collision box of the last call
							 * to test_collision_with_map() returning true */ 

    bool default_ignore_obstacles;			/**< indicates that this movement normally ignores obstacles */
    bool current_ignore_obstacles;			/**< indicates that this movement currently ignores obstacles */

    // properties
    std::map<std::string, std::string> properties;	/**< a key-value map of properties, used to handle the movement from a script
    							 * (each subclass accepts a precise list of keys) */

  protected:

    Movement(bool ignore_obstacles = false);

    // suspended
    uint32_t get_when_suspended();

    // obstacles
    void set_default_ignore_obstacles(bool ignore_obstacles);

  public:

    virtual ~Movement();

    // entity
    virtual void set_entity(MapEntity *entity);

    // update
    virtual void update(); // called repeatedly

    // suspended
    bool is_suspended();
    virtual void set_suspended(bool suspended);

    // position
    int get_x();
    int get_y();
    const Rectangle& get_xy();
    void set_x(int x);
    void set_y(int y);
    void set_xy(const Rectangle &xy);
    void translate_x(int dx);
    void translate_y(int dy);
    void translate_xy(const Rectangle &dxy);
    virtual void notify_position_changed(); // called whenever x or y is changed

    // movement
    virtual bool is_stopped();
    virtual bool is_started();
    virtual bool is_finished();
    virtual void stop();

    // obstacles
    bool test_collision_with_obstacles(int dx, int dy);
    bool test_collision_with_obstacles(const Rectangle &dxy);
    const Rectangle& get_last_collision_box_on_obstacle();
    bool are_obstacles_ignored();
    void set_ignore_obstacles(bool ignore_obstacles);
    void restore_default_ignore_obstacles();

    // properties
    const std::string& get_property(const std::string &key);
    void set_property(const std::string &key, const std::string &value);
};

#endif

