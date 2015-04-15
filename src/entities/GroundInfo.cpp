/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/GroundInfo.h"
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

namespace {

const std::map<Ground, std::string> ground_names = {
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

}  // Anonymous namespace.

namespace GroundInfo {

/**
 * \brief Returns whether a ground value represents a diagonal obstacle.
 * \return ground The ground to test.
 * \retrun \c true if this is diagonal ground.
 */
bool is_ground_diagonal(Ground ground) {

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

/**
 * \brief Returns the ground values and their Lua name.
 * \return The name of each ground.
 */
const std::map<Ground, std::string>& get_ground_names() {
  return ground_names;
}

/**
 * \brief Returns the name of a ground.
 * \param type A ground value.
 * \return The corresponding name.
 */
const std::string& get_ground_name(Ground ground) {

  return ground_names.at(ground);
}

/**
 * \brief Returns the ground value with the given name.
 * \param ground_name The name of a ground. It must exist.
 * \return The corresponding ground value.
 */
Ground get_ground_by_name(const std::string& ground_name) {

  for (const auto& kvp: ground_names) {
    if (kvp.second == ground_name) {
      return kvp.first;
    }
  }

  Debug::die(std::string("Unknown ground: ") + ground_name);
  return Ground();
}

}

}
