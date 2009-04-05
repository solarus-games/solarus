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
#ifndef ZSDX_TILESET_H
#define ZSDX_TILESET_H

#include "Common.h"
#include "Color.h"

/**
 * A tileset is an image with a set of elements (tile patterns)
 * one can use to compose a map. * 
 * See the directory images/tilesets.
 */
class Tileset {

 private:

  TilesetId id;                     /**< id of the tileset */
  TilePattern *tile_patterns[1024]; /**< tile patterns in this tileset (the maximum id is 1024) */
  int nb_tile_patterns;             /**< number of tile patterns in the tileset */
  int max_tile_id;                  /**< current maximum id of a tile pattern in this tileset */
  Uint32 background_color;          /**< background color of the tileset */
  SDL_Surface *tiles_image;         /**< image from which the tile patterns are extracted */
  SDL_Surface *entities_image;      /**< image from which the skin-dependent entities are extracted */

  void add_tile_pattern(int id, TilePattern *tile_pattern);

 public:

  Tileset(TilesetId id);
  ~Tileset(void);

  void load(void);
  void unload(void);

  Uint32 get_background_color(void);
  bool is_loaded(void);
  SDL_Surface *get_tiles_image(void);
  SDL_Surface *get_entities_image(void);
  TilePattern *get_tile_pattern(int id);
};

#endif
