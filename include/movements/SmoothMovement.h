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
#ifndef SOLARUS_SMOOTH_MOVEMENT_H
#define SOLARUS_SMOOTH_MOVEMENT_H

#include "Common.h"
#include "movements/RectilinearMovement.h"

/**
 * @brief Rectilinear movement capable of fixing the trajectory to continue walking when
 * an obstacle is reached.
 */
class SmoothMovement: public RectilinearMovement {

  private:

    bool smooth;		/**< indicates that the movement fixes its trajectory
				 * (if not, the movement is identical to a normal RectilinearMovement) */

  public:

    SmoothMovement(bool smooth = true);
    virtual ~SmoothMovement();

    void update_x();
    void update_y();

    bool is_smooth();
    void set_smooth(bool smooth);
};

#endif

