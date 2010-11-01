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
#ifndef SOLARUS_PATH_MOVEMENT_H
#define SOLARUS_PATH_MOVEMENT_H

#include "Common.h"
#include "movements/RectilinearMovement.h"

/**
 * @brief Movement of an entity that follows a predetermined path.
 *
 * The path is a succession of basic moves, where each basic move
 * is an 8-pixel movement towards one of the 8 main directions.
 * The movement may or may not stop on obstacles.
 */
class PathMovement: public PixelMovement {

  private:

    std::string initial_path;		/**< the path: each character is a direction ('0' to '7')
    					 * and corresponds to a move of 8 pixels */
    std::string remaining_path;		/**< the remaining part of the path */

    int speed;				/**< the movement speed in pixels per second */
    bool loop;				/**< should the movement return to the beginning once finished?  */

    bool snap_to_grid;			/**< indicates that the entity must be aligned to the grid before moving */

    // snapping
    bool snapping;			/**< indicates that the movement is currently snapping the entity on the grid */
    uint32_t stop_snapping_date;	/**< date when we stop trying to snap the entity when it is unsuccessful */

  public:

    PathMovement(const std::string &path, int speed, bool loop, bool ignore_obstacles, bool snap_to_grid);
    ~PathMovement();

    virtual bool is_finished();

    void update();
};

#endif

