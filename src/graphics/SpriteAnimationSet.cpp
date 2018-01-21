/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Rectangle.h"
#include "solarus/graphics/SpriteAnimation.h"
#include "solarus/graphics/SpriteAnimationSet.h"
#include "solarus/graphics/SpriteAnimationDirection.h"
#include "solarus/graphics/SpriteData.h"
#include "solarus/lua/LuaTools.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace Solarus {

/**
 * \brief Loads the animations of a sprite from a file.
 * \param id Id of the sprite animation set to load
 * (name of a sprite definition file, without the ".dat" extension).
 */
SpriteAnimationSet::SpriteAnimationSet(const std::string& id):
  id(id) {

  load();
}

/**
 * \brief Attempts to load this animation set from its file.
 */
void SpriteAnimationSet::load() {

  Debug::check_assertion(animations.empty(),
      "Animation set already loaded");

  // Load the sprite data file.
  std::string file_name = std::string("sprites/") + id + ".dat";
  SpriteData data;
  bool success = data.import_from_quest_file(file_name);
  if (success) {
    // Get the imported data.
    default_animation_name = data.get_default_animation_name();
    for (const auto& kvp : data.get_animations()) {
      add_animation(kvp.first, kvp.second);
    }
  }
}

/**
 * \brief Adds a new animation to this animation set.
 *
 * This function is called while loading the animation set.
 *
 * \param animation_name Name of this animation.
 * \param animation_data Properties of the animation to create.
 */
void SpriteAnimationSet::add_animation(
    const std::string& animation_name,
    const SpriteAnimationData& animation_data) {

  std::string src_image = animation_data.get_src_image();
  uint32_t frame_delay = animation_data.get_frame_delay();
  int frame_to_loop_on = animation_data.get_loop_on_frame();
  std::vector<SpriteAnimationDirection> directions;

  // Create directions
  for (const SpriteAnimationDirectionData& direction: animation_data.get_directions()) {

    Size size = direction.get_size();
    max_size.width = std::max(size.width, max_size.width);
    max_size.height = std::max(size.height, max_size.height);
    max_bounding_box |= direction.get_bounding_box();

    directions.emplace_back(direction.get_all_frames(), direction.get_origin());
  }

  animations.emplace(
    animation_name,
    SpriteAnimation(src_image, directions, frame_delay, frame_to_loop_on)
  );
}

/**
 * \brief When the sprite is displayed on a map, sets the tileset.
 *
 * This function must be called if this sprite image depends on the map's tileset.
 *
 * \param tileset The tileset.
 */
void SpriteAnimationSet::set_tileset(const Tileset& tileset) {

  for (auto& kvp: animations) {
    kvp.second.set_tileset(tileset);
  }
}

/**
 * \brief Returns whether this animation set has an animation with the specified name.
 * \param animation_name an animation name
 * \return true if this animation exists
 */
bool SpriteAnimationSet::has_animation(
    const std::string& animation_name) const {
  return animations.find(animation_name) != animations.end();
}

/**
 * \brief Returns an animation.
 * \param animation_name Name of the animation to get.
 * \return The specified animation.
 */
const SpriteAnimation& SpriteAnimationSet::get_animation(
    const std::string& animation_name) const {

  Debug::check_assertion(has_animation(animation_name),
      std::string("No animation '") + animation_name
      + "' in animation set '" + id + "'"
  );

  return animations.find(animation_name)->second;
}

/**
 * \brief Returns an animation.
 * \param animation_name Name of the animation to get.
 * \return The specified animation.
 */
SpriteAnimation& SpriteAnimationSet::get_animation(
    const std::string& animation_name) {

  Debug::check_assertion(has_animation(animation_name),
      std::string("No animation '") + animation_name
      + "' in animation set '" + id + "'"
  );

  return animations.find(animation_name)->second;
}

/**
 * \brief Returns the name of the default animation, i.e. the first one.
 * \return the name of the default animation
 */
const std::string& SpriteAnimationSet::get_default_animation() const {
  return default_animation_name;
}

/**
 * \brief Enables the pixel-perfect collision detection for these animations.
 */
void SpriteAnimationSet::enable_pixel_collisions() {

  if (!are_pixel_collisions_enabled()) {

    for (auto& kvp: animations) {
      kvp.second.enable_pixel_collisions();
    }
  }
}

/**
 * \brief Returns whether the pixel-perfect collisions are enabled for these animations.
 * \return true if the pixel-perfect collisions are enabled
 */
bool SpriteAnimationSet::are_pixel_collisions_enabled() const {

  if (animations.empty()) {
    return false;
  }

  return animations.begin()->second.are_pixel_collisions_enabled();
}

/**
 * \brief Returns a size big enough to contain any frame of this animation set.
 * \return The maximum size of a frame in this animation set.
 */
const Size& SpriteAnimationSet::get_max_size() const {
  return max_size;
}

/**
 * \brief Returns a rectangle big enough to contain a frame of any animation
 * and direction.
 *
 * The rectangle is anchored to the origin point of the sprite,
 * so it has has negative top and left coordinates.
 *
 * \return The maximum frame size.
 */
const Rectangle& SpriteAnimationSet::get_max_bounding_box() const {
  return max_bounding_box;
}

}

