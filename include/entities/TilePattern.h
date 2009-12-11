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
#ifndef ZSDX_TILE_PATTERN_H
#define ZSDX_TILE_PATTERN_H

#include "Common.h"
#include "entities/Obstacle.h"

/**
 * Abstract class for a tile pattern.
 * A tile pattern defines a rectangle image in a tileset and has 
 * an obstacle property.
 * Examples of subclasses are SimpleTilePattern and AnimatedTilePattern.
 * The width and the height of a tile pattern are always multiples or 8.
 */
class TilePattern {
  
 protected:

  const Obstacle obstacle; /**< is the tile an obstacle? */

  const int width;         /**< tile width (multiple of 8) */
  const int height;        /**< tile height (multiple of 8) */

  TilePattern(Obstacle obstacle, int width, int height);

 public:

  virtual ~TilePattern(void);

  int get_width(void) const;
  int get_height(void) const;
  Obstacle get_obstacle(void) const;

  static void update(void);
  void display_on_map(Map *map, const Rectangle &position_in_map);
  virtual void display(Surface *surface, const Rectangle &position_in_surface, Surface *tileset_image) = 0;
};

#endif

