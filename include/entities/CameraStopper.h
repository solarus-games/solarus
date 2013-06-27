/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_CAMERA_STOPPER_H
#define SOLARUS_CAMERA_STOPPER_H

#include "Common.h"
#include "entities/Detector.h"
#include <list>

/**
 * \brief An horizontal or vertical, invisible wall for the camera.
 *
 * The camera cannot cross this wall except when the hero touches it: in this
 * case, scrolling is performed.
 * This entity allows to separate parts of the map like if there was several
 * maps.
 */
class CameraStopper: public Detector {

  public:

    CameraStopper(
        const std::string& name,
        Layer layer,
        int x,
        int y,
        int width,
        int height);
    ~CameraStopper();

    EntityType get_type();

};

#endif

