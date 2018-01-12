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
#include "solarus/entities/CollisionMode.h"

namespace Solarus {

const std::string EnumInfoTraits<CollisionMode>::pretty_name = "collision mode";

const EnumInfo<CollisionMode>::names_type EnumInfoTraits<CollisionMode>::names = {
  { CollisionMode::COLLISION_NONE, "none" },
  { CollisionMode::COLLISION_CUSTOM, "custom" },
  { CollisionMode::COLLISION_OVERLAPPING, "overlapping" },
  { CollisionMode::COLLISION_CONTAINING, "containing" },
  { CollisionMode::COLLISION_FACING, "facing" },
  { CollisionMode::COLLISION_ORIGIN, "origin" },
  { CollisionMode::COLLISION_TOUCHING, "touching" },
  { CollisionMode::COLLISION_CENTER, "center" },
  { CollisionMode::COLLISION_SPRITE, "sprite" },
};

const EnumInfo<CollisionMode>::names_type EnumInfoTraits<CollisionMode>::names_no_none_no_custom = {
  { CollisionMode::COLLISION_OVERLAPPING, "overlapping" },
  { CollisionMode::COLLISION_CONTAINING, "containing" },
  { CollisionMode::COLLISION_FACING, "facing" },
  { CollisionMode::COLLISION_ORIGIN, "origin" },
  { CollisionMode::COLLISION_TOUCHING, "touching" },
  { CollisionMode::COLLISION_CENTER, "center" },
  { CollisionMode::COLLISION_SPRITE, "sprite" }
};

}
