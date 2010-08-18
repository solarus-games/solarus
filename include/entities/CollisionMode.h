/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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

/**
 * @brief Collisions modes of a detector.
 *
 * The collision mode indicates what kind of collisions a detector will detect.
 */
enum CollisionMode {
  COLLISION_NONE         = 0x0000, /**< no collision will be detected
				    * (the detector doesn't detect any entity) */
  COLLISION_RECTANGLE    = 0x0001, /**< collision if the entity's rectangle
				    * overlaps the detector's rectangle */
  COLLISION_INSIDE       = 0x0002, /**< collision if the entity's rectangle
				    * is inside the detector's rectangle */
  COLLISION_ORIGIN_POINT = 0x0004, /**< collision if the entity's origin point
				    * is inside the detector's rectangle */
  COLLISION_FACING_POINT = 0x0008, /**< collision if the entity's facing point
				    * is inside the detector's rectangle */
  COLLISION_CENTER       = 0x0010, /**< collision if the entity's center
				    * is inside the detector's rectangle */
  COLLISION_SPRITE       = 0x0020, /**< collision if an entity's sprite has pixels
				    * overlapping pixels of the detector's sprite */
  COLLISION_CUSTOM       = 0x0040  /**< custom collision function, defined by a subclass of Detector */
};

#endif

