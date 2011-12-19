/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_GROUND_H
#define SOLARUS_GROUND_H

/**
 * @brief The possible kinds of ground on the map.
 */
enum Ground {
  GROUND_NORMAL,
  GROUND_GRASS,
  GROUND_SHALLOW_WATER,
  GROUND_DEEP_WATER,
  GROUND_HOLE,
  GROUND_PRICKLE,
  GROUND_LAVA,
  GROUND_LADDER,
  GROUND_EMPTY
};

#endif

