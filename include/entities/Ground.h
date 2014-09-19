/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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

namespace Solarus {

/**
 * \brief The possible kinds of grounds on a map.
 *
 * The ground of a point of the map is the one of the tile, dynamic tile
 * or destructible entity placed there.
 * If several entities are present at that point (like a traversable dynamic
 * tile over a hole tile),
 * the highest one whose ground is not GROUND_EMPTY wins.
 */
enum Ground {

  GROUND_EMPTY,                   /**< Keep unchanged the ground of entities below. */
  GROUND_TRAVERSABLE,             /**< Normal travserable ground. */
  GROUND_WALL,                    /**< Obstacle entity. */
  GROUND_LOW_WALL,                /**< Can only be traversed by projectiles like arrows,
                                   * boomerang, thrown items, etc. */
  GROUND_WALL_TOP_RIGHT,          /**< The upper-right half of the entity is an obstacle. */
  GROUND_WALL_TOP_LEFT,           /**< The upper-left half of the entity is an obstacle. */
  GROUND_WALL_BOTTOM_LEFT,        /**< The lower-left half of the entity is an obstacle. */
  GROUND_WALL_BOTTOM_RIGHT,       /**< The lower-right half of the entity is an obstacle. */
  GROUND_WALL_TOP_RIGHT_WATER,    /**< The upper-right half of the entity is an obstacle
                                   * and the rest is deep water. */
  GROUND_WALL_TOP_LEFT_WATER,     /**< The upper-left half of the entity is an obstacle
                                   * and the rest is deep water. */
  GROUND_WALL_BOTTOM_LEFT_WATER,  /**< The lower-left half of the entity is an obstacle
                                   * and the rest is deep water. */
  GROUND_WALL_BOTTOM_RIGHT_WATER, /**< The lower-right half of the entity is an obstacle
                                   * and the rest is deep water. */
  GROUND_DEEP_WATER,              /**< Deep water where the hero drowns or swims. */
  GROUND_SHALLOW_WATER,           /**< Shallow water where the hero can walk but slower. */
  GROUND_GRASS,                   /**< Grass where the hero walks slower. */
  GROUND_HOLE,                    /**< A hole where the hero can fall. */
  GROUND_ICE,                     /**< Ice terrain where the hero slides. */
  GROUND_LADDER,                  /**< A ladder that slows down the hero. */
  GROUND_PRICKLE,                 /**< Some untraversable prickles that hurt the hero. */
  GROUND_LAVA,                    /**< Some lava where the hero drowns and gets hurt. */
};

}

#endif

