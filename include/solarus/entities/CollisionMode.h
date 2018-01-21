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
#ifndef SOLARUS_COLLISION_MODE_H
#define SOLARUS_COLLISION_MODE_H

#include "solarus/core/EnumInfo.h"

namespace Solarus {

/**
 * \brief Collisions modes an entity can detect.
 */
enum CollisionMode {
  COLLISION_NONE              = 0x0000, /**< No collision will be detected
                                         * (the detector doesn't detect any entity). */
  COLLISION_OVERLAPPING       = 0x0001, /**< Collision if the entity's rectangle
                                         * overlaps the detector's rectangle. */
  COLLISION_CONTAINING        = 0x0002, /**< Collision if the entity's rectangle
                                         * is inside the detector's rectangle. */
  COLLISION_ORIGIN            = 0x0004, /**< Collision if the entity's origin point
                                         * is inside the detector's rectangle. */
  COLLISION_FACING            = 0x0008, /**< Collision if the entity's facing point corresponding to
                                         * its current direction is inside the detector's rectangle. */
  COLLISION_TOUCHING          = 0x0010, /**< Collision if any entity's facing point (i.e. in any of
                                         * the four main directions) is inside the detector's rectangle. */
  COLLISION_CENTER            = 0x0020, /**< Collision if the entity's center point
                                         * is inside the detector's rectangle. */
  COLLISION_SPRITE            = 0x0040, /**< Collision if an entity's sprite has pixels
                                         * overlapping pixels of the detector's sprite. */
  COLLISION_CUSTOM            = 0x0080  /**< Custom collision function, defined by a subclass of Entity. */
};

template <>
struct SOLARUS_API EnumInfoTraits<CollisionMode> {
  static const std::string pretty_name;

  static const EnumInfo<CollisionMode>::names_type names;
  static const EnumInfo<CollisionMode>::names_type names_no_none_no_custom;
};

}

#endif
