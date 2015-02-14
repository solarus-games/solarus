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
#ifndef SOLARUS_LAYER_H
#define SOLARUS_LAYER_H

namespace Solarus {

/**
 * \brief Layer of an entity.
 *
 * The layer represents the height of the entity on the map.
 * Any map entity can be on any layer.
 * On each layer, entities are placed in the order they were created.
 */
enum Layer {
  LAYER_LOW,             /**< Low layer. */
  LAYER_INTERMEDIATE,    /**< Intermediate layer. */
  LAYER_HIGH,            /**< High layer. */
  LAYER_NB               /**< Number of layers. */
};

}

#endif

