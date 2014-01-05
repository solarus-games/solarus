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

namespace solarus {

/**
 * \brief Constructor.
 * \param map The map (not loaded yet).
 * \param layer The layer to represent.
 */
NonAnimatedRegions::NonAnimatedRegions(Map& map, Layer layer):
  map(map),
  layer(layer),
  non_animated_tiles_surface(NULL) {

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

  RefCountable::unref(non_animated_tiles_surface);
  non_animated_tiles_surface = NULL;
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

  Debug::check_assertion(non_animated_tiles_surface == NULL,
      "Tile regions are alrady built");
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

  Debug::check_assertion(non_animated_tiles_surface == NULL,
      "Tile regions are alrady built");

  const Rectangle map_size(0, 0, map.get_width(), map.get_height());
  const int map_width8 = map.get_width8();
  const int map_height8 = map.get_height8();

  // Initialize the are_squares_animated booleans to false.
  for (int i = 0; i < map_width8 * map_height8; ++i) {
    are_squares_animated.push_back(false);
  }

  // Create the surface where all non-animated tiles will be drawn.
  non_animated_tiles_surface = Surface::create(
      map_size.get_width(), map_size.get_height()
  );
  RefCountable::ref(non_animated_tiles_surface);

  // Set this surface as a software destination because it is built only
  // once and never changes later.
  non_animated_tiles_surface->set_software_destination(true);

  for (unsigned i = 0; i < tiles.size(); ++i) {
    Tile& tile = *tiles[i];
    if (!tile.is_animated()) {
      // Non-animated tile: optimize its displaying.
      tile.draw(*non_animated_tiles_surface, map_size);
    }
    else {
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
        non_animated_tiles_surface->clear(animated_square);
      }
      index++;
    }
  }

  // Build the list of animated tiles and tiles overlapping them.
  std::vector<Tile*> accepted_tiles;
  for (unsigned i = 0; i < tiles.size(); ++i) {
    Tile& tile = *tiles[i];
    if (tile.is_animated() || overlaps_animated_tile(tile)) {
      // Sorry, this tile cannot be optimized.
      rejected_tiles.push_back(&tile);
    }
    else {
      accepted_tiles.push_back(&tile);
    }
  }

  // No need to keep all tiles at this point. Just keep the optimized ones.
  tiles = accepted_tiles;
}

/**
 * \brief Clears previous drawings because the tileset has changed.
 */
void NonAnimatedRegions::notify_tileset_changed() {

  if (non_animated_tiles_surface == NULL) {
    // Regions are not built yet.
    return;
  }

  const Rectangle map_size(0, 0, map.get_width(), map.get_height());
  for (int layer = 0; layer < LAYER_NB; layer++) {

    non_animated_tiles_surface->clear();

    for (unsigned i = 0; i < tiles.size(); ++i) {
      // The tiles list only contains the optimized ones.
      tiles[i]->draw(*non_animated_tiles_surface, map_size);
    }

    // Erase rectangles that contain animated tiles.
    int index = 0;
    for (int y = 0; y < map.get_height(); y += 8) {
      for (int x = 0; x < map.get_width(); x += 8) {

        if (are_squares_animated[index]) {
          Rectangle animated_square(x, y, 8, 8);
          non_animated_tiles_surface->clear(animated_square);
        }
        index++;
      }
    }
  }
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

  non_animated_tiles_surface->draw_region(
      map.get_camera_position(), map.get_visible_surface()
  );
}

}

