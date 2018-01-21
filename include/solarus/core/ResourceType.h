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
#ifndef SOLARUS_RESOURCE_TYPE_H
#define SOLARUS_RESOURCE_TYPE_H

#include "solarus/core/Common.h"

namespace Solarus {

/**
 * \brief Types of existing resources in a quest.
 */
enum class ResourceType {
  MAP,
  TILESET,
  SPRITE,
  MUSIC,
  SOUND,
  ITEM,
  ENEMY,
  ENTITY,
  LANGUAGE,
  FONT,
  SHADER,
};

}

#endif

