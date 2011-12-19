/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_PATH_MOVEMENT_H
#define SOLARUS_PATH_MOVEMENT_H

#include "Common.h"
#include "movements/PixelMovement.h"

/**
 * @brief Movement of an entity that follows a predetermined path.
 *
 * The path is a succession of basic moves, where each basic move
 * is an 8-pixel trajectory towards one of the 8 main directions.
 * The movement may or may not stop on obstacles.
 * This movement can be used only on a map entity.
 *
 * A "path" refers to a succession of small 8-pixel trajectories.
 * The notion of path is specific to PathMovement and its subclasses (like PathFindingMovement and RandomPathMovement).
 * A "trajectory" is a move of 8 pixels in the sense of the PixelMovement class.
 * A path is composed of several trajectories. The notion of trajectory is hidden from the public interface of PathMovement.
 *
 * Properties:
 * - path
 * - speed
 * - loop
 * - ignore_obstacles
 * - snap_to_grid
 */
class PathMovement: public PixelMovement {

  private:

    std::string initial_path;					/**< the path: each character is a direction ('0' to '7')
								* and corresponds to a trajectory of 8 pixels (performed by PixelMovement) */
    std::string remaining_path;					/**< the remaining part of the path */
    int current_direction;					/**< current element in the path (0 to 7) */
    int total_distance_covered;					/**< total number of pixels covered (each element of the path counts for 8) */
    bool stopped_by_obstacle;					/**< true if the movement was stopped by an obstacle */

    int speed;							/**< the movement speed in pixels per second (corrected for diagonal moves) */
    bool loop;							/**< should the path restart from the beginning once finished? */

    // snapping
    bool snap_to_grid;						/**< indicates that the entity must be aligned to the grid before moving */
    bool snapping;						/**< indicates that the entity is currently being aligned to the grid */
    uint32_t stop_snapping_date;				/**< date when we stop trying to snap the entity if it is unsuccessful */

    static const std::string elementary_moves[];		/**< 8 pixel trajectory (in the PixelMovement sense) for each direction (0 to 7) */

  private:

    static uint32_t speed_to_delay(int speed, int direction);

    void start_next_elementary_move();
    bool is_current_elementary_move_finished();

    void snap();
    void set_snapping_trajectory(const Rectangle &src, const Rectangle &dst);

  protected:

    void notify_step_done(int step_index, bool success);

  public:

    PathMovement(const std::string &path, int speed, bool loop, bool ignore_obstacles, bool snap_to_grid);
    ~PathMovement();

    void set_entity(MapEntity *entity);
    virtual void update();
    virtual void set_suspended(bool suspended);
    virtual bool is_finished();
    void restart();

    void set_path(const std::string &path);
    int get_speed();
    void set_speed(int speed);
    bool get_loop();
    void set_loop(bool loop);
    void set_snap_to_grid(bool snap_to_grid);

    Rectangle get_xy_change();
    int get_current_direction();
    int get_total_distance_covered();
    int get_displayed_direction4();

    static const std::string create_random_path();

    // properties
    virtual const std::string get_property(const std::string &key);
    virtual void set_property(const std::string &key, const std::string &value);
};

#endif

