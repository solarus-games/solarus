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
#include "lowlevel/System.h"

namespace solarus {

/**
 * \brief Creates a stream action.
 * \param stream The stream applied.
 * \param entity_moved Entity the stream is applied to.
 */
StreamAction::StreamAction(Stream& stream, MapEntity& entity_moved):
  stream(&stream),
  entity_moved(&entity_moved),
  active(true),
  suspended(false) {

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
 * \brief Returns the stream responsible from this action.
 * \return The stream.
 */
const Stream& StreamAction::get_stream() const {
  return *stream;
}

/**
 * \brief Returns the entity this action is applied to.
 * \return The entity moved by the stream.
 */
const MapEntity& StreamAction::get_entity_moved() const {
  return *entity_moved;
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

  // First check that the stream and the entity still exist
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

  if (is_suspended()) {
    return;
  }

  /* TODO
    // see if the stream's movement is finished
    if (hero.get_movement()->is_finished() || !stream.overlaps(hero)) {

      hero.set_state(new FreeState(hero));
    }
    else {
      // update the sprites direction
      int keys_direction8 = get_commands().get_wanted_direction8();
      int movement_direction8 = stream.get_direction();

      int animation_direction = get_sprites().get_animation_direction(keys_direction8, movement_direction8);
      if (animation_direction != get_sprites().get_animation_direction()
          && animation_direction != -1) {
        get_sprites().set_animation_direction(animation_direction);
      }
    }
    */
}

/**
 * \brief Returns whether this stream action is currently suspended.
 * \return \c true if the stream action is suspended.
 */
bool StreamAction::is_suspended() const {
  return suspended;
}

/**
 * \brief Suspends or resumes this stream action.
 * \param suspended \c true to suspend, \c false to resume.
 */
void StreamAction::set_suspended(bool suspended) {

  this->suspended = suspended;
  if (suspended) {
    when_suspended = System::now();
  }
}

}

