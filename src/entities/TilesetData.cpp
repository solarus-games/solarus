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
#include "solarus/entities/TilesetData.h"
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

/**
 * \brief Creates a default single-frame tile pattern.
 */
TilePatternData::TilePatternData() :
    TilePatternData(Rectangle(0, 0, 16, 16)) {
}

/**
 * \brief Creates a single-frame tile pattern.
 * \param frame Coordinates of the single frame to make.
 */
TilePatternData::TilePatternData(const Rectangle& frame) :
    ground(Ground::TRAVERSABLE),
    default_layer(LAYER_LOW),
    scrolling(TileScrolling::NONE),
    frames() {

  set_frame(frame);
}

/**
 * \brief Returns the kind of ground of this pattern.
 * \return The ground.
 */
Ground TilePatternData::get_ground() const {
  return ground;
}

/**
 * \brief Sets the kind of ground of this pattern.
 * \param ground The ground.
 */
void TilePatternData::set_ground(Ground ground) {
  this->ground = ground;
}

/**
 * \brief Returns the initial layer when creating a tile with this pattern.
 * \return The default layer.
 */
Layer TilePatternData::get_default_layer() const {
  return default_layer;
}

/**
 * \brief Sets the initial layer when creating a tile with this pattern.
 * \param default_layer The default layer.
 */
void TilePatternData::set_default_layer(Layer default_layer) {
  this->default_layer = default_layer;
}

/**
 * \brief Returns the kind of scrolling of this pattern.
 * \return The scrolling property.
 */
TileScrolling TilePatternData::get_scrolling() const {
  return scrolling;
}

/**
 * \brief Sets the kind of scrolling of this pattern.
 * \param scrolling The scrolling property.
 */
void TilePatternData::set_scrolling(TileScrolling scrolling) {
  this->scrolling = scrolling;
}

/**
 * \brief Returns whether this is a single-frame pattern.
 * \return \c true if the pattern has only one frame.
 */
bool TilePatternData::is_single_frame() const {
  return frames.size() <= 1;
}

/**
 * \brief Returns the coordinates of this pattern in the tileset image file.
 *
 * This only makes sense for single-frame patterns.
 *
 * \return The coordinates of the single frame of this pattern.
 * Returns an empty rectangle if the pattern is nil.
 */
Rectangle TilePatternData::get_frame() const {

  return frames[0];
}

/**
 * \brief Sets the coordinates of this pattern in the tileset image file.
 *
 * The pattern will have a single frame.
 *
 * \param frame The coordinates of the single frame of this pattern.
 */
void TilePatternData::set_frame(const Rectangle& frame) {
  this->frames.clear();
  this->frames.push_back(frame);
}

/**
 * \brief Returns the coordinates of each frame of this pattern in the tileset
 * image file.
 * \return The coordinates of the frame(s) of this pattern.
 */
const std::vector<Rectangle>& TilePatternData::get_frames() const {
  return frames;
}

/**
 * \brief Sets the coordinates of each frame of this pattern in the tileset
 * image file.
 * \param frames The coordinates of the frame(s) of this pattern.
 */
void TilePatternData::set_frames(const std::vector<Rectangle>& frames) {

  Debug::check_assertion(!frames.empty(), "No frames");
  // TODO check number of elements
  this->frames = frames;
}

/**
 * \brief Creates an empty tileset.
 */
TilesetData::TilesetData() :
    background_color(Color::white),
    patterns() {

}

/**
 * \brief Loads a tileset data file from memory.
 * \param buffer A memory area with the content of a tileset data file.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
bool TilesetData::import_from_buffer(const std::string& /* buffer */) {
  // TODO
  return false;
}

/**
 * \brief Loads a tileset data file from the filesystem.
 * \param file_name Path of the file to load.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
bool TilesetData::import_from_file(const std::string& /* file_name */) {
  // TODO
  return false;
}

/**
 * \brief Saves this tileset data file into memory.
 * \param buffer The buffer to write.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
bool TilesetData::export_to_buffer(std::string& /* buffer */) const {
  // TODO
  return false;
}

/**
 * \brief Saves the tileset data into a file.
 * \param file_name Path of the file to save.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
bool TilesetData::export_to_file(const std::string& /* file_name */) const {
  // TODO
  return false;
}

/**
 * \brief Returns the tileset's background color.
 * \return The background color.
 */
Color TilesetData::get_background_color() const {
  return background_color;
}

/**
 * \brief Sets the tileset's background color.
 * \param background_color The background color.
 */
void TilesetData::set_background_color(const Color& background_color) {
  this->background_color = background_color;
}

/**
 * \brief Returns all tile patterns of this tileset.
 * \return The tile pattern indexed by their id.
 */
const std::map<std::string, TilePatternData>& TilesetData::get_patterns() const {
  return patterns;
}

/**
 * \brief Returns whether there exists a tile pattern with the specified id.
 * \param pattern_id The id to test.
 * \return \c true if a tile pattern exists with this id in the tileset.
 */
bool TilesetData::exists(const std::string& pattern_id) const {
  return patterns.find(pattern_id) != patterns.end();
}

/**
 * \brief Returns the pattern with the specified id.
 * \param pattern_id A tile pattern id. It must exist.
 * \return The pattern with this id.
 */
const TilePatternData& TilesetData::get_pattern(const std::string& pattern_id) const {

  const auto& it = patterns.find(pattern_id);
  Debug::check_assertion(it != patterns.end(),
    std::string("No such tile pattern: '") + pattern_id + "'");

  return it->second;
}

/**
 * \brief Returns the pattern with the specified id.
 *
 * Non-const version.
 *
 * \param pattern_id A tile pattern id. It must exist.
 * \return The pattern with this id.
 */
TilePatternData& TilesetData::get_pattern(const std::string& pattern_id) {

  const auto& it = patterns.find(pattern_id);
  Debug::check_assertion(it != patterns.end(),
    std::string("No such tile pattern: '") + pattern_id + "'");

  return it->second;
}

/**
 * \brief Adds a pattern to the tileset.
 * \param pattern_id Id of the new pattern.
 * \param pattern The pattern to add.
 * \return \c true in case of success.
 */
bool TilesetData::add_pattern(
    const std::string& pattern_id, const TilePatternData& pattern) {

  const auto& result = patterns.insert(std::make_pair(pattern_id, pattern));
  if (!result.second) {
    // Insertion failed: the id already exists.
    return false;
  }

  return true;
}

/**
 * \brief Removes a pattern from the tileset.
 * \param pattern_id Id of the pattern to remove.
 * \return \c true in case of success.
 */
bool TilesetData::remove_pattern(const std::string& pattern_id) {

  return patterns.erase(pattern_id) > 0;
}

/**
 * \brief Changes the id of a pattern in the tileset.
 * \param old_pattern_id Old id of the pattern.
 * \param new_pattern_id New id to set.
 * \return \c true in case of success.
 * In case of failure, the old pattern is unchanged.
 */
bool TilesetData::set_pattern_id(
    const std::string& old_pattern_id, const std::string& new_pattern_id) {

  if (!exists(old_pattern_id)) {
    // No pattern was found with the old id.
    return false;
  }

  if (exists(new_pattern_id)) {
    // The new id is already used.
    return false;
  }

  TilePatternData pattern = get_pattern(old_pattern_id);
  remove_pattern(old_pattern_id);
  add_pattern(new_pattern_id, pattern);

  return true;
}


}
