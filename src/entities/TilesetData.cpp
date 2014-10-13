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

namespace Solarus {

/**
 * \brief Creates a default tile pattern.
 */
TilePatternData::TilePatternData():
  ground(Ground::TRAVERSABLE),
  scrolling(TileScrolling::NONE),
  frames() {

  set_frame(Rectangle(0, 0, 16, 16));
}

Ground TilePatternData::get_ground() const {
  return ground;
}

void TilePatternData::set_ground(Ground ground) {
  this->ground = ground;
}

Layer TilePatternData::get_default_layer() const {
  return default_layer;
}

void TilePatternData::set_default_layer(Layer default_layer) {
  this->default_layer = default_layer;
}

TileScrolling TilePatternData::get_scrolling() const {
  return scrolling;
}

void TilePatternData::set_scrolling(TileScrolling scrolling) {
  this->scrolling = scrolling;
}

const Rectangle& TilePatternData::get_frame() const {
  return frames[0];
}

void TilePatternData::set_frame(const Rectangle& frame) {
  this->frames.clear();
  this->frames.push_back(frame);
}

const std::vector<Rectangle>& TilePatternData::get_frames() const {
  return frames;
}

void TilePatternData::set_frames(const std::vector<Rectangle>& frames) {
  // TODO check size
  this->frames = frames;
}

}
