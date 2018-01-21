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
#include "solarus/audio/Sound.h"
#include "solarus/core/CommandsEffects.h"
#include "solarus/core/Game.h"
#include "solarus/core/Map.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Random.h"
#include "solarus/core/System.h"
#include "solarus/entities/Crystal.h"
#include "solarus/entities/Hero.h"
#include "solarus/graphics/Sprite.h"
#include "solarus/graphics/SpriteAnimationSet.h"
#include <lua.hpp>
#include <memory>

namespace Solarus {

/**
 * \brief Creates a new crystal.
 * \param name name of the entity to create
 * \param layer layer of the entity to create on the map
 * \param xy Coordinates of the entity to create.
 */
Crystal::Crystal(const std::string& name, int layer, const Point& xy):
  Entity(name, 0, layer, xy, Size(16, 16)),
  state(false),
  next_possible_hit_date(System::now()) {

  set_collision_modes(
      CollisionMode::COLLISION_SPRITE |
      CollisionMode::COLLISION_OVERLAPPING |
      CollisionMode::COLLISION_FACING
  );
  set_origin(8, 13);
  main_sprite = create_sprite("entities/crystal", "main");
  main_sprite->enable_pixel_collisions();
  star_sprite = create_sprite("entities/star", "star");
  star_sprite->enable_pixel_collisions();
  twinkle();
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Crystal::get_type() const {
  return ThisType;
}

/**
 * \copydoc Entity::notify_creating
 */
void Crystal::notify_creating() {

  Entity::notify_creating();

  bool state = get_game().get_crystal_state();
  if (state != this->state) {

    this->state = state;
    main_sprite->set_current_animation(state ? "blue_lowered" : "orange_lowered");
  }
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other another entity
 * \return true if this entity is an obstacle for the other one
 */
bool Crystal::is_obstacle_for(Entity& other) {
  return other.is_crystal_obstacle(*this);
}

/**
 * \brief This function is called when another entity collides with this crystal.
 * \param entity_overlapping the other entity
 * \param collision_mode the collision mode that detected the collision
 */
void Crystal::notify_collision(Entity& entity_overlapping, CollisionMode collision_mode) {
  entity_overlapping.notify_collision_with_crystal(*this, collision_mode);
}

/**
 * \copydoc Entity::notify_collision(Entity&, Sprite&, Sprite&)
 */
void Crystal::notify_collision(Entity& other_entity, Sprite& this_sprite, Sprite& other_sprite) {

  if (&this_sprite == main_sprite.get()) {
    other_entity.notify_collision_with_crystal(*this, other_sprite);
  }
}

/**
 * \copydoc Entity::notify_action_command_pressed
 */
bool Crystal::notify_action_command_pressed() {

  if (get_hero().is_free() &&
      get_commands_effects().get_action_key_effect() == CommandsEffects::ACTION_KEY_LOOK
  ) {
    get_commands_effects().set_action_key_effect(CommandsEffects::ACTION_KEY_NONE);

    // start a dialog
    get_game().start_dialog("_crystal", ScopedLuaRef(), ScopedLuaRef());
    return true;
  }

  return false;
}

/**
 * \brief Activates the crystal if the delay since the last activation allows it.
 * \param entity_activating the entity that activates this crystal
 */
void Crystal::activate(Entity& entity_activating) {

  bool recently_activated = false;
  for (Entity* entity: entities_activating) {
    if (entity == &entity_activating) {
      recently_activated = true;
      break;
    }
  }

  uint32_t now = System::now();
  if (!recently_activated || now >= next_possible_hit_date) {
    Sound::play("switch");
    get_game().change_crystal_state();
    next_possible_hit_date = now + 1000;
    entities_activating.push_back(&entity_activating);
  }
}

/**
 * \brief Makes a star twinkle on the crystal at a random position.
 */
void Crystal::twinkle() {

  const Point star_xy = { Random::get_number(3, 13), Random::get_number(3, 13) };
  star_sprite->restart_animation();
  star_sprite->set_xy(star_xy - get_origin());
}

/**
 * \brief Updates the entity.
 */
void Crystal::update() {

  if (!is_suspended()) {
    bool state = get_game().get_crystal_state();
    if (state != this->state) {

      this->state = state;
      main_sprite->set_current_animation(state ? "blue_lowered" : "orange_lowered");
    }

    star_sprite->update();
    if (star_sprite->is_animation_finished()) {
      twinkle();
    }

    uint32_t now = System::now();
    if (now >= next_possible_hit_date) {
      entities_activating.clear();
    }
  }

  Entity::update();
}

/**
 * \brief Suspends or resumes the entity.
 * \param suspended true to suspend the entity, false to resume it
 */
void Crystal::set_suspended(bool suspended) {

  Entity::set_suspended(suspended);

  if (!suspended) {
    next_possible_hit_date += System::now() - get_when_suspended();
  }
}

}

