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
#ifndef SOLARUS_LAYER_H
#define SOLARUS_LAYER_H

/**
 * @brief Layer of an entity.
 *
 * The layer represents the height or the entity in the map.
 */
enum Layer {
  LAYER_LOW,             /**< the entity is always below the hero (floor, walls, chests,
			      enemies and 99% of the entities) */
  LAYER_INTERMEDIATE,    /**< the hero can be below or above the entity (platforms or objects on a plaform) */
  LAYER_HIGH,            /**< the entity is always above the hero (trees, top of doors…) */
  LAYER_NB               /**< number of layers */
};

#endif

