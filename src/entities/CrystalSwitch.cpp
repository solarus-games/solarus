/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/CrystalSwitch.h"
#include "ZSDX.h"
#include "Game.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
using std::string;

/**
 * Creates a new crystal switch.
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 */
CrystalSwitch::CrystalSwitch(Layer layer, int x, int y):
  Detector(COLLISION_SPRITE, "", layer, x, y, 16, 16),
  state(false), next_possible_hit_date(SDL_GetTicks()) {

  create_sprite("entities/crystal_switch");
  get_sprite()->get_animation_set()->enable_pixel_collisions();
}

/**
 * Destructor.
 */
CrystalSwitch::~CrystalSwitch(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType CrystalSwitch::get_type() {
  return CRYSTAL_SWITCH;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool CrystalSwitch::is_obstacle_for(MapEntity *other) {
  return true;
}

/**
 * This function is called by the engine when a sprite overlaps the crystal switch.
 * If the entity is the hero, we allow him to lift the item.
 * @param entity an entity
 * @param sprite_overlapping the sprite of this entity that is overlapping the detector
 */
void CrystalSwitch::collision(MapEntity *entity, Sprite *sprite_overlapping) {

  if (entity->is_hero() &&
      sprite_overlapping->get_animation_set_id().find("sword") != string::npos) {
    // the hero's sword is overlapping the crystal switch

    Uint32 now = SDL_GetTicks();

    if (now >= next_possible_hit_date && is_hit_by_sword((Hero*) entity)) {
      // TODO sound
      zsdx->game->change_crystal_switch_state();
      next_possible_hit_date = now + 500;
    }
  }
}

/**
 * Updates the entity.
 */
void CrystalSwitch::update(void) {

  bool state = zsdx->game->get_crystal_switch_state();
  if (state != this->state) {

    this->state = state;
    get_sprite()->set_current_animation(state ? "blue_lowered" : "orange_lowered");
  }
  MapEntity::update();
}

/**
 * Suspends or resumes the entity.
 * @param suspended true to suspend the entity, false to resume it
 */
void CrystalSwitch::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  if (!suspended) {
    next_possible_hit_date += SDL_GetTicks() - when_suspended;
  }
}
