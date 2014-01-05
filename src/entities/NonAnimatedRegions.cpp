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
#include "entities/NonAnimatedRegions.h"
#include "entities/MapEntities.h"
#include "entities/Tile.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"
#include "RefCountable.h"
#include "Map.h"

/* TODO still to be fixed:
 * - Need to erase animated tiles regions.
 * - What about tiles that overlap each other at the frontier of cells?
 */

namespace solarus {

/**
 * \brief Constructor.
 * \param map The map. Its size must be known.
 * \param layer The layer to represent.
 */
NonAnimatedRegions::NonAnimatedRegions(Map& map, Layer layer):
  map(map),
  layer(layer),
  optimized_tiles_surface(NULL),
  optimized_tiles(map.get_size(), Rectangle(0, 0, 400, 300)) {

}

/**
 * \brief Destructor.
 */
NonAnimatedRegions::~NonAnimatedRegions() {

  clear();
}

/**
 * \brief Cleans everything.
 */
void NonAnimatedRegions::clear() {

  RefCountable::unref(optimized_tiles_surface);
  optimized_tiles_surface = NULL;
  are_squares_animated.clear();

  for (unsigned i = 0; i < tiles.size(); ++i) {
    map.get_entities().destroy_entity(tiles[i]);
  }
  tiles.clear();
}

/**
 * \brief Adds a tile to the list of tiles.
 */
void NonAnimatedRegions::add_tile(Tile* tile) {

  Debug::check_assertion(optimized_tiles_surface == NULL,
      "Tile regions are already built");
  Debug::check_assertion(tile != NULL, "Missing tile");
  Debug::check_assertion(tile->get_layer() == layer, "Wrong layer for add tile");

  tiles.push_back(tile);
}

/**
 * \brief Determines which rectangles are animated to allow drawing all non-animated
 * rectangles of tiles only once.
 * \param[out] rejected_tiles The list of tiles that are in animated regions.
 * They include all animated tiles plus the static tiles overlapping them.
 */
void NonAnimatedRegions::build(std::vector<Tile*>& rejected_tiles) {

  Debug::check_assertion(optimized_tiles_surface == NULL,
      "Tile regions are already built");

  const Rectangle map_size(0, 0, map.get_width(), map.get_height());
  const int map_width8 = map.get_width8();
  const int map_height8 = map.get_height8();

  // Initialize the are_squares_animated booleans to false.
  for (int i = 0; i < map_width8 * map_height8; ++i) {
    are_squares_animated.push_back(false);
  }

  // Create the surface where all non-animated tiles will be drawn.
  optimized_tiles_surface = Surface::create(
      map_size.get_width(), map_size.get_height()
  );
  RefCountable::ref(optimized_tiles_surface);

  // Set this surface as a software destination because it is built only
  // once and never changes later.
  optimized_tiles_surface->set_software_destination(true);

  for (unsigned i = 0; i < tiles.size(); ++i) {
    Tile& tile = *tiles[i];
    if (tile.is_animated()) {
      // Animated tile: mark its region as non-optimizable
      // (otherwise, a non-animated tile above an animated one would screw us).

      int tile_x8 = tile.get_x() / 8;
      int tile_y8 = tile.get_y() / 8;
      int tile_width8 = tile.get_width() / 8;
      int tile_height8 = tile.get_height() / 8;

      for (int i = 0; i < tile_height8; i++) {
        for (int j = 0; j < tile_width8; j++) {

          int x8 = tile_x8 + j;
          int y8 = tile_y8 + i;
          if (x8 >= 0 && x8 < map_width8 && y8 >= 0 && y8 < map_height8) {
            int index = y8 * map_width8 + x8;
            are_squares_animated[index] = true;
          }
        }
      }
    }
  }

  // Erase rectangles that contain animated tiles.
  int index = 0;
  for (int y = 0; y < map.get_height(); y += 8) {
    for (int x = 0; x < map.get_width(); x += 8) {

      if (are_squares_animated[index]) {
        Rectangle animated_square(x, y, 8, 8);
        optimized_tiles_surface->clear(animated_square);
      }
      index++;
    }
  }

  // Build the list of animated tiles and tiles overlapping them.
  for (unsigned i = 0; i < tiles.size(); ++i) {
    Tile& tile = *tiles[i];
    if (tile.is_animated() || overlaps_animated_tile(tile)) {
      // Sorry, this tile cannot be optimized.
      rejected_tiles.push_back(&tile);
    }
    else {
      optimized_tiles.add(&tile);
    }
  }

  // No need to keep all tiles at this point. Just keep the optimized ones.
  tiles.clear();
}

/**
 * \brief Clears previous drawings because the tileset has changed.
 */
void NonAnimatedRegions::notify_tileset_changed() {

  optimized_tiles_surface->clear();
  grid_cells_drawn.clear();  // Everything will be redrawn when necessary.
}

/**
 * \brief Returns whether a tile is overlapping an animated other tile.
 * \param tile The tile to check.
 * \return \c true if this tile is overlapping an animated tile.
 */
bool NonAnimatedRegions::overlaps_animated_tile(Tile& tile) const {

  if (tile.get_layer() != layer) {
    return false;
  }

  int tile_x8 = tile.get_x() / 8;
  int tile_y8 = tile.get_y() / 8;
  int tile_width8 = tile.get_width() / 8;
  int tile_height8 = tile.get_height() / 8;

  for (int i = 0; i < tile_height8; i++) {
    for (int j = 0; j < tile_width8; j++) {

      int x8 = tile_x8 + j;
      int y8 = tile_y8 + i;
      if (x8 >= 0 && x8 < map.get_width8() && y8 >= 0 && y8 < map.get_height8()) {

        int index = y8 * map.get_width8() + x8;
        if (are_squares_animated[index]) {
          return true;
        }
      }
    }
  }
  return false;
}

/**
 * \brief Draws a layer of non animated regions on the current map.
 */
void NonAnimatedRegions::draw_on_map() {

  if (grid_cells_drawn.size() < optimized_tiles.get_num_cells()) {
    // Not everything is drawn yet.
    // Make sure the grid cells in the camera are drawn.
    ensure_tiles_drawn(map.get_camera_position());
  }

  // At this point, at least all tiles in the camera are drawn on optimized_tiles_surface.
  optimized_tiles_surface->draw_region(
      map.get_camera_position(), map.get_visible_surface()
  );
}

/**
 * \brief Makes sures that at least all tiles in the specified rectangle are
 * drawn on the surface.
 * \param where The area to draw.
 */
void NonAnimatedRegions::ensure_tiles_drawn(const Rectangle& where) {

  // Check all cells that overlap the rectangle.
  const int num_rows = optimized_tiles.get_num_rows();
  const int num_columns = optimized_tiles.get_num_columns();
  const Rectangle& cell_size(optimized_tiles.get_cell_size());
  const Rectangle& map_size = map.get_size();

  const int row1 = where.get_y() / cell_size.get_height();
  const int row2 = (where.get_y() + where.get_height()) / cell_size.get_height();
  const int column1 = where.get_x() / cell_size.get_width();
  const int column2 = (where.get_x() + where.get_width()) / cell_size.get_width();

  if (row1 > row2 || column1 > column2) {
    // No cell.
    return;
  }

  for (int i = row1; i <= row2; ++i) {
    if (i < 0 || i >= num_rows) {
      continue;
    }

    for (int j = column1; j <= column2; ++j) {
      if (j < 0 || j >= num_columns) {
        continue;
      }

      int cell_index = i * num_columns + j;
      if (grid_cells_drawn.find(cell_index) == grid_cells_drawn.end()) {
        // This cell is not drawn yet. Let's draw it.
        grid_cells_drawn.insert(cell_index);
        const std::vector<Tile*> tiles_in_cell =
            optimized_tiles.get_elements(cell_index);
        std::vector<Tile*>::const_iterator it;
        for (it = tiles_in_cell.begin(); it != tiles_in_cell.end(); ++it) {
          Tile& tile = *(*it);
          tile.draw(*optimized_tiles_surface, map_size);
        }
      }
    }
  }
}

}

