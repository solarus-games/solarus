/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/entities/GroundInfo.h"

namespace Solarus {

const std::string EnumInfoTraits<Ground>::pretty_name = "ground";

const EnumInfo<Ground>::names_type EnumInfoTraits<Ground>::names = {
    { Ground::EMPTY, "empty" } ,
    { Ground::TRAVERSABLE, "traversable" },
    { Ground::WALL, "wall" },
    { Ground::LOW_WALL, "low_wall" },
    { Ground::WALL_TOP_RIGHT, "wall_top_right" },
    { Ground::WALL_TOP_LEFT, "wall_top_left" },
    { Ground::WALL_BOTTOM_LEFT, "wall_bottom_left" },
    { Ground::WALL_BOTTOM_RIGHT, "wall_bottom_right" },
    { Ground::WALL_TOP_RIGHT_WATER, "wall_top_right_water" },
    { Ground::WALL_TOP_LEFT_WATER, "wall_top_left_water" },
    { Ground::WALL_BOTTOM_LEFT_WATER, "wall_bottom_left_water" },
    { Ground::WALL_BOTTOM_RIGHT_WATER, "wall_bottom_right_water" },
    { Ground::DEEP_WATER, "deep_water" },
    { Ground::SHALLOW_WATER, "shallow_water" },
    { Ground::GRASS, "grass" },
    { Ground::HOLE, "hole" },
    { Ground::ICE, "ice" },
    { Ground::LADDER, "ladder" },
    { Ground::PRICKLE, "prickles" },
    { Ground::LAVA, "lava" }
};

/**
 * \brief Returns whether a ground value represents a diagonal obstacle.
 * \return ground The ground to test.
 * \retrun \c true if this is diagonal ground.
 */
bool GroundInfo::is_ground_diagonal(Ground ground) {

  switch (ground) {

  case Ground::WALL_TOP_RIGHT:
  case Ground::WALL_TOP_LEFT:
  case Ground::WALL_BOTTOM_LEFT:
  case Ground::WALL_BOTTOM_RIGHT:
  case Ground::WALL_TOP_RIGHT_WATER:
  case Ground::WALL_TOP_LEFT_WATER:
  case Ground::WALL_BOTTOM_LEFT_WATER:
  case Ground::WALL_BOTTOM_RIGHT_WATER:
    return true;

  case Ground::EMPTY:
  case Ground::TRAVERSABLE:
  case Ground::WALL:
  case Ground::LOW_WALL:
  case Ground::DEEP_WATER:
  case Ground::SHALLOW_WATER:
  case Ground::GRASS:
  case Ground::HOLE:
  case Ground::ICE:
  case Ground::LADDER:
  case Ground::PRICKLE:
  case Ground::LAVA:
    return false;

  }

  return false;
}

}
