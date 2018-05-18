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
#include "solarus/entities/Tileset.h"
#include "solarus/graphics/SpriteAnimation.h"
#include "solarus/graphics/SpriteAnimationDirection.h"
#include "solarus/graphics/Surface.h"
#include <sstream>

namespace Solarus {

/**
 * \brief Constructor.
 * \param image_file_name The image from which the frames are extracted.
 * \param directions The image sequence of each direction.
 * \param frame_delay Delay in millisecond between two frames for this sprite animation
 * (or 0 to make no animation, for example when you have only one frame).
 * \param loop_on_frame Frame to loop on after the last frame (or -1 to make no loop).
 */
SpriteAnimation::SpriteAnimation(
    const std::string& image_file_name,
    const std::vector<SpriteAnimationDirection>& directions,
    uint32_t frame_delay,
    int loop_on_frame):

  src_image(nullptr),
  src_image_is_tileset(image_file_name == "tileset"),
  directions(directions),
  frame_delay(frame_delay),
  loop_on_frame(loop_on_frame),
  should_enable_pixel_collisions(false) {

  if (!src_image_is_tileset) {
    src_image = Surface::create(image_file_name);
    if (src_image == nullptr) {
      Debug::error(std::string("Cannot load sprite image '" + image_file_name + "'"));
    };
  }
}

/**
 * \brief When the sprite is displayed on a map, sets the tileset.
 *
 * This function must be called if this sprite image depends on the map's tileset.
 *
 * \param tileset The tileset.
 */
void SpriteAnimation::set_tileset(const Tileset& tileset) {

  if (!src_image_is_tileset) {
    // Nothing to do when the tileset changes.
    return;
  }

  src_image = tileset.get_entities_image();
  if (src_image == nullptr) {
    std::string file_name = std::string("tilesets/") + tileset.get_id() + ".entities.png";
    Debug::error(std::string("Missing sprites image for tileset '") + tileset.get_id() + "': " + file_name);
  }

  if (should_enable_pixel_collisions) {
    disable_pixel_collisions();  // To force creating the images again.
    do_enable_pixel_collisions();
  }
}

/**
 * \brief Returns the number of directions of this animation.
 * \return The number of directions.
 */
int SpriteAnimation::get_nb_directions() const {
  return directions.size();
}

/**
 * \brief Returns the delay between two frames for this sprite animation.
 * \return the frame delay in milliseconds
 */
uint32_t SpriteAnimation::get_frame_delay() const {
  return frame_delay;
}

/**
 * \brief Returns whether this animation loops on a frame.
 * \return true if this animation loops
 */
bool SpriteAnimation::is_looping() const {
  return loop_on_frame != -1;
}

/**
 * \brief Returns the next frame of the current frame.
 * \param current_direction the current direction
 * \param current_frame the current frame
 * \return the next frame of the current frame in this direction
 * (or -1 if the animation is over)
 */
int SpriteAnimation::get_next_frame(
    int current_direction, int current_frame) const {

  if (current_direction < 0
      || current_direction >= get_nb_directions()) {
    std::ostringstream oss;
    oss << "Invalid sprite direction '" << current_direction
        << "': this sprite has " << get_nb_directions()
        << " direction(s)";
    Debug::die(oss.str());
  }

  int next_frame = current_frame + 1;

  if (next_frame == directions[current_direction].get_nb_frames()) {
    // If we are on the last frame
    // we loop on the appropriate frame
    // or -1 if there is no loop.
    next_frame = loop_on_frame;
  }

  return next_frame;
}

/**
 * \brief Draws a specific frame of this animation on a surface.
 * \param dst_surface the surface on which the sprite will be drawn
 * \param dst_position coordinates on the destination surface
 * (the origin point will be drawn at this position)
 * \param current_direction the direction to show
 * \param current_frame the frame to show in this direction
 * \param infos draw infos bundle
 */
void SpriteAnimation::draw(Surface& dst_surface,
    const Point& dst_position, int current_direction, int current_frame, const DrawInfos &infos) const {

  if (src_image == nullptr) {
    return;
  }

  if (current_direction < 0
      || current_direction >= get_nb_directions()) {
    std::ostringstream oss;
    oss << "Invalid sprite direction "
        << current_direction << ": this sprite has " << get_nb_directions()
        << " direction(s)";
    Debug::die(oss.str());
  }
  directions[current_direction].draw(dst_surface, dst_position,
      current_frame, *src_image, infos);
}

/**
 * \brief Enables the pixel-perfect collision detection for this animation.
 */
void SpriteAnimation::enable_pixel_collisions() {

  if (src_image != nullptr) {
    do_enable_pixel_collisions();
  }
  else {
    // Wait for the source image to be available before analyzing its pixels.
    should_enable_pixel_collisions = true;
  }
}

/**
 * \brief Internal function that enables the pixel-perfect collision detection for this animation now.
 */
void SpriteAnimation::do_enable_pixel_collisions() {

  if (src_image == nullptr) {
    return;
  }

  for (SpriteAnimationDirection& direction: directions) {
    direction.enable_pixel_collisions(*src_image);
  }
}

/**
 * \brief Disables the pixel-perfect collision detection for this animation.
 */
void SpriteAnimation::disable_pixel_collisions() {

  if (src_image == nullptr) {
    return;
  }

  for (SpriteAnimationDirection& direction: directions) {
    direction.disable_pixel_collisions();
  }
}

/**
 * \brief Returns whether the pixel-perfect collisions are enabled for this animations.
 * \return true if the pixel-perfect collisions are enabled
 */
bool SpriteAnimation::are_pixel_collisions_enabled() const {

  if (should_enable_pixel_collisions) {
    return true;
  }

  if (directions.empty()) {
    return false;
  }

  return directions[0].are_pixel_collisions_enabled();
}

}

