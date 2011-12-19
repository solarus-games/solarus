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
#include "entities/ParallaxScrollingTilePattern.h"
#include "entities/Tileset.h"
#include "lowlevel/Surface.h"

/**
 * @brief Distance made by the viewport to move the tile pattern of 1 pixel.
 */
const int ParallaxScrollingTilePattern::ratio = 2;

/**
 * @brief Creates a tile pattern with parallax scrolling.
 * @param obstacle is the tile pattern an obstacle?
 * @param x x position of the tile pattern in the tileset
      else if (animation == 2) {
        pattern = new ParallaxScrollingTilePattern(Obstacle(obstacle), x, y, width, height);
      }
 * @param y y position of the tile pattern in the tileset
 * @param width width of the tile pattern in the tileset
 * @param height height of the tile pattern in the tileset
 */
ParallaxScrollingTilePattern::ParallaxScrollingTilePattern(Obstacle obstacle, int x, int y, int width, int height):
  SimpleTilePattern(obstacle, x, y, width, height) {

}

/**
 * @brief Destructor.
 */
ParallaxScrollingTilePattern::~ParallaxScrollingTilePattern() {

}

/**
 * @brief Displays the tile image on a surface.
 * @param dst_surface the surface to draw
 * @param dst_position position where tile pattern should be displayed on dst_surface
 * @param tileset the tileset of this tile
 * @param viewport coordinates of the top-left corner of dst_surface relative
 * to the map (may be used for scrolling tiles)
 */
void ParallaxScrollingTilePattern::display(Surface* dst_surface, const Rectangle& dst_position,
    Tileset& tileset, const Rectangle& viewport) {

  Surface* tileset_image = tileset.get_tiles_image();
  Rectangle dst(dst_position);
  dst.add_xy(viewport.get_x() / ratio, viewport.get_y() / ratio);
  tileset_image->blit(position_in_tileset, dst_surface, dst);

  // one day, we can implement several scrolling layers just by changing the ratio
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
bool ParallaxScrollingTilePattern::is_animated() {
  return true;
}

/**
 * @brief Returns whether tiles having this tile pattern are displayed at their
 * position.
 *
 * Usually, this function returns true, and when it is the case, display() is
 * called only for tiles that are located in the current viewport.
 *
 * However, some tile patterns may want to be displayed even when they are not
 * in the viewport, typically to make an illusion of movement like parallax
 * scrolling.
 *
 * @return true to if this tile pattern is always displayed at its coordinates
 */
bool ParallaxScrollingTilePattern::is_displayed_at_its_position() {
  return false;
}

