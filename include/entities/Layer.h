/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_LAYER_H
#define ZSDX_LAYER_H

/**
 * Layer of a tile or a dynamic entity.
 */
enum Layer {
  LAYER_LOW,             /**< the entity is always below the hero (floor, walls, chests,
			      enemies and 99% of the tiles and active objects) */
  LAYER_INTERMEDIATE,    /**< the hero can be below or above the entity (platforms or objects on a plaform) */
  LAYER_HIGH,            /**< the entity is always above the hero (trees, top of doors...) */
  LAYER_NB               /**< number of layers */
};

#endif
