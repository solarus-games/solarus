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
#include "solarus/core/AbilityInfo.h"

namespace Solarus {

const std::string EnumInfoTraits<Ability>::pretty_name = "ability";

const EnumInfo<Ability>::names_type EnumInfoTraits<Ability>::names = {
    { Ability::TUNIC, "tunic" },
    { Ability::SWORD, "sword" },
    { Ability::SWORD_KNOWLEDGE, "sword_knowledge" },
    { Ability::SHIELD, "shield" },
    { Ability::LIFT, "lift" },
    { Ability::SWIM, "swim" },
    { Ability::JUMP_OVER_WATER, "jump_over_water" },
    { Ability::RUN, "run" },
    { Ability::PUSH, "push" },
    { Ability::GRAB, "grab" },
    { Ability::PULL, "pull" },
    { Ability::DETECT_WEAK_WALLS, "detect_weak_walls" }
};

}
