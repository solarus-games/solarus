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
#include "solarus/entities/AnimatedTilePattern.h"
#include "solarus/entities/GroundInfo.h"
#include "solarus/entities/ParallaxScrollingTilePattern.h"
#include "solarus/entities/SelfScrollingTilePattern.h"
#include "solarus/entities/SimpleTilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/entities/TilesetData.h"
#include "solarus/entities/TimeScrollingTilePattern.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lua/LuaData.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

/**
 * \brief Constructor.
 * \param id id of the tileset to create
 */
Tileset::Tileset(const std::string& id):
  id(id),
  tiles_image(nullptr),
  entities_image(nullptr) {
}

/**
 * \brief Returns the id of this tileset.
 * \return the tileset id
 */
const std::string& Tileset::get_id() {
  return id;
}

/**
 * \brief Adds a new tile pattern to this tileset.
 *
 * This function is called while loading the tileset.
 *
 * \param id Id of this tile pattern.
 * \param pattern_data Properties of the tile pattern to create.
 */
void Tileset::add_tile_pattern(
    const std::string& id,
    const TilePatternData& pattern_data
) {
  // Create the tile pattern.
  TilePattern* tile_pattern = nullptr;

  const std::vector<Rectangle>& frames = pattern_data.get_frames();
  const TileScrolling scrolling = pattern_data.get_scrolling();
  const Ground ground = pattern_data.get_ground();

  if (frames.size() == 1) {
    // Single frame.
    const Rectangle& frame = frames[0];

    switch (scrolling) {

    case TileScrolling::NONE:
      tile_pattern = new SimpleTilePattern(
          ground, frame.get_xy(), frame.get_size()
      );
      break;

    case TileScrolling::PARALLAX:
      tile_pattern = new ParallaxScrollingTilePattern(
          ground, frame.get_xy(), frame.get_size()
      );
      break;

    case TileScrolling::SELF:
      tile_pattern = new SelfScrollingTilePattern(
          ground, frame.get_xy(), frame.get_size()
      );
      break;
    }
  }
  else {
    // Multi-frame.
    if (scrolling == TileScrolling::SELF) {
      Debug::error("Multi-frame is not supported for self-scrolling tiles");
      return;
    }

    bool parallax = scrolling == TileScrolling::PARALLAX;
    AnimatedTilePattern::AnimationSequence sequence = (frames.size() == 3) ?
        AnimatedTilePattern::ANIMATION_SEQUENCE_012 : AnimatedTilePattern::ANIMATION_SEQUENCE_0121;
    tile_pattern = new AnimatedTilePattern(
        ground,
        sequence,
        frames[0].get_size(),
        frames[0].get_x(),
        frames[0].get_y(),
        frames[1].get_x(),
        frames[1].get_y(),
        frames[2].get_x(),
        frames[2].get_y(),
        parallax
    );
  }

  tile_patterns.insert(std::make_pair(id, std::unique_ptr<TilePattern>(tile_pattern)));
}

/**
 * \brief Loads the tileset from its file by creating all tile patterns.
 */
void Tileset::load() {

  // Load the tileset data file.
  std::string file_name = std::string("tilesets/") + id + ".dat";
  TilesetData data;
  bool success = data.import_from_quest_file(file_name);
  if (success) {
    // Get the imported data.
    this->background_color = data.get_background_color();
    for (const auto& kvp : data.get_patterns()) {
      add_tile_pattern(kvp.first, kvp.second);
    }
  }

  // Load the tileset images.
  file_name = std::string("tilesets/") + id + ".tiles.png";
  tiles_image = Surface::create(file_name, Surface::DIR_DATA);
  if (tiles_image == nullptr) {
    Debug::error(std::string("Missing tiles image for tileset '") + id + "': " + file_name);
    tiles_image = Surface::create(16, 16);
  }

  file_name = std::string("tilesets/") + id + ".entities.png";
  entities_image = Surface::create(file_name, Surface::DIR_DATA);
  if (entities_image == nullptr) {
    Debug::error(std::string("Missing entities image for tileset '") + id + "': " + file_name);
    entities_image = Surface::create(16, 16);
  }
}

/**
 * \brief Destroys the tile patterns and frees the memory used
 * by the tileset image.
 */
void Tileset::unload() {

  tile_patterns.clear();
  tiles_image = nullptr;
  entities_image = nullptr;
}

/**
 * \brief Returns the background color of this tileset.
 * \return the background color
 */
Color& Tileset::get_background_color() {
  return background_color;
}

/**
 * \brief Returns whether this tileset is loaded.
 * \return true if this tileset is loaded
 */
bool Tileset::is_loaded() {
  return tiles_image != nullptr;
}

/**
 * \brief Returns the image containing the tiles of this tileset.
 * \return the tiles image
 */
const SurfacePtr& Tileset::get_tiles_image() {
  return tiles_image;
}

/**
 * \brief Returns the image containing the skin-dependent dynamic entities for this tileset.
 * \return the image containing the skin-dependent dynamic entities for this tileset
 */
const SurfacePtr& Tileset::get_entities_image() {
  return entities_image;
}

/**
 * \brief Returns a tile pattern from this tileset.
 * \param id id of the tile pattern to get
 * \return the tile pattern with this id
 */
TilePattern& Tileset::get_tile_pattern(const std::string& id) {

  const auto& it = tile_patterns.find(id);
  if (it == tile_patterns.end()) {
    std::ostringstream oss;
    oss << "No such tile pattern in tileset '" << get_id() << "': " << id;
    Debug::die(oss.str());
  }
  return *it->second;
}

/**
 * \brief Changes the tiles images, the entities images and the background
 * color of this tileset.
 * \param other_id Id of another tileset whose images and background color
 * will be copied into this tileset.
 */
void Tileset::set_images(const std::string& other_id) {

  // Load the new tileset to take its images.
  Tileset tmp_tileset(other_id);
  tmp_tileset.load();

  tiles_image = tmp_tileset.get_tiles_image();
  entities_image = tmp_tileset.get_entities_image();

  background_color = tmp_tileset.get_background_color();
}

}

