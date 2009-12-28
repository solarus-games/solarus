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
#ifndef ZSDX_PATH_MOVEMENT_H
#define ZSDX_PATH_MOVEMENT_H

#include "Common.h"
#include "movements/CollisionMovement.h"

/**
 * Movement for an entity that follows a path.
 * The path is a succession of basic moves, where
 * each basic move is an 8-pixel movement in one
 * of the 8 main directions.
 */
class PathMovement: public CollisionMovement {

  protected:

    const std::string initial_path;  /**< the path: each character is a direction ('0' to '7') */
    std::string remaining_path;      /**< the remaining part of the path */
    int normal_speed;                /**< the movement speed */

  private:

    static const std::string random_directions[];

    int current_direction;           /**< direction of the current move (0 to 7) */
    int distance_covered;            /**< counts the number of pixels covered during the current move */
    int total_distance_covered;      /**< counts the total number of pixels covered since the beginning of this movement */

    const bool loop;                 /**< should the movement return to the beginning once finished?  */

    bool finished;                   /**< indicates that the path is finished
				      * (possible if loop is false or when there is a collision) */
    bool must_be_aligned;            /**< indicates that the entity must be snapped to the grid before moving */
    bool snapping;                   /**< indicates that the movement is currently snapping the entity on the grid */
    double snapping_angle;           /**< angle of the movement when snapping */
    uint32_t stop_snapping_date;     /**< date when we stop trying to snap the entity when it is unsuccessful */

  protected:

    virtual void start_next_move(void);
    static const std::string get_random_path(void);
    bool is_current_move_finished(void);
    void set_entity(MapEntity *entity);
    void set_speed(int speed);

  public:

    PathMovement(const std::string &path, int speed, bool loop, bool with_collisions, bool snap_to_grid);
    ~PathMovement(void);

    virtual bool is_finished(void);

    int get_current_direction(void);
    void set_position(int x, int y);
    int get_total_distance_covered(void);

    void update(void);
};

#endif

