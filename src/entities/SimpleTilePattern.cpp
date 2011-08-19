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
#include "entities/SimpleTilePattern.h"
#include "entities/Tileset.h"
#include "lowlevel/Surface.h"
#include "Map.h"

/**
 * @brief Creates a simple tile pattern.
 * @param obstacle is the tile pattern an obstacle?
 * @param x x position of the tile pattern in the tileset
 * @param y y position of the tile pattern in the tileset
 * @param width width of the tile pattern in the tileset
 * @param height height of the tile pattern in the tileset
 */
SimpleTilePattern::SimpleTilePattern(Obstacle obstacle, int x, int y, int width, int height):
  TilePattern(obstacle, width, height), position_in_tileset(x, y, width, height) {

}

/**
 * Destructor.
 */
SimpleTilePattern::~SimpleTilePattern() {

}

/**
 * @brief Displays the tile image on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset the tileset of this tile
 */
void SimpleTilePattern::display(Surface *destination, const Rectangle &dst_position, Tileset &tileset) {

  Surface *tileset_image = tileset.get_tiles_image();
  tileset_image->blit(position_in_tileset, destination, dst_position);
}

/**
 * @brief Returns whether this tile pattern is animated, i.e. not always displayed
 * the same way.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 *
 * @return true if this tile pattern is animated
 */
bool SimpleTilePattern::is_animated() {
  return false;
}

