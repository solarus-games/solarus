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
#include "solarus/entities/StartingLocationMode.h"

namespace Solarus {

const std::string EnumInfoTraits<StartingLocationMode>::pretty_name = "starting location mode";

const EnumInfo<StartingLocationMode>::names_type EnumInfoTraits<StartingLocationMode>::names = {
    { StartingLocationMode::WHEN_WORLD_CHANGES, "when_world_changes" } ,
    { StartingLocationMode::YES, "yes" },
    { StartingLocationMode::NO, "no" }
};

}
