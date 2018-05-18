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
#include "solarus/graphics/TransitionImmediate.h"
#include "solarus/graphics/Surface.h"

namespace Solarus {

/**
 * \brief Creates an immediate transition effect.
 * \param direction direction of the transition effect (opening or closing)
 */
TransitionImmediate::TransitionImmediate(Transition::Direction direction):
  Transition(direction) {
}

/**
 * \brief Starts this transition effect.
 */
void TransitionImmediate::start() {

}

/**
 * \brief Returns whether the transition effect is started.
 * \return false
 */
bool TransitionImmediate::is_started() const {
  return false;
}

/**
 * \brief Returns whether the transition effect is finished.
 * \return true
 */
bool TransitionImmediate::is_finished() const {
  return true;
}

/**
 * \brief Notifies the transition effect that it was just suspended
 * or resumed.
 * \param suspended true if suspended, false if resumed.
 */
void TransitionImmediate::notify_suspended(bool /* suspended */) {
}

/**
 * \brief Updates this transition effect.
 */
void TransitionImmediate::update() {

}

/**
 * \brief Draws the transition effect on a surface.
 * \param dst_surface the surface to draw
 */
void TransitionImmediate::draw(Surface& dst_surface, const Surface &src_surface, const DrawInfos &infos) const {
  infos.proxy.draw(dst_surface,src_surface,infos);
}

}

