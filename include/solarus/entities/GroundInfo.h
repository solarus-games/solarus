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
#ifndef SOLARUS_GROUND_INFO_H
#define SOLARUS_GROUND_INFO_H

#include "solarus/Common.h"
#include "solarus/entities/Ground.h"
#include <map>
#include <string>

namespace Solarus {

namespace GroundInfo {

SOLARUS_API bool is_ground_diagonal(Ground ground);

SOLARUS_API const std::map<Ground, std::string>& get_ground_names();
SOLARUS_API const std::string& get_ground_name(Ground ground);
SOLARUS_API Ground get_ground_by_name(const std::string& ground_name);

}

}

#endif

