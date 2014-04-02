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
#include "entities/StreamAction.h"
#include "entities/Stream.h"

namespace solarus {

/**
 * \brief Creates a stream action.
 * \param stream The stream applied.
 * \param entity_moved Entity the stream is applied to.
 */
StreamAction::StreamAction(Stream& stream, MapEntity& entity_moved):
  stream(&stream),
  entity_moved(&entity_moved),
  active(true) {

  RefCountable::ref(this->stream);
  RefCountable::ref(this->entity_moved);
}

/**
 * \brief Destructor.
 */
StreamAction::~StreamAction() {

  RefCountable::unref(this->stream);
  RefCountable::unref(this->entity_moved);
}

/**
 * \brief Returns whether the action is currently active.
 * The action is inactive if the stream effect was completely applied,
 * or if the stream of the entity are destroyed or disabled.
 */
bool StreamAction::is_active() const {
  return active;
}

/**
 * \brief Called repeatedly by the main loop.
 * Updates the effect of the stream on the entity.
 */
void StreamAction::update() {

  // If the action is disabled, do nothing.
  if (!is_active()) {
    return;
  }

  // First check that the stream or the entity still exist
  // and are enabled.
  if (stream->is_being_removed()) {
    RefCountable::unref(stream);
    stream = NULL;
    active = false;
  }
  else if (!stream->is_enabled()) {
    active = false;
  }

  if (entity_moved->is_being_removed()) {
    RefCountable::unref(entity_moved);
    entity_moved = NULL;
    active = false;
  }
  else if (!entity_moved->is_enabled()) {
    active = false;
  }
  if (!is_active()) {
    return;
  }
}

}

