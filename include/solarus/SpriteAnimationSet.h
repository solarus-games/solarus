/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_SPRITE_ANIMATION_SET_H
#define SOLARUS_SPRITE_ANIMATION_SET_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Size.h"
#include <map>
#include <string>

struct lua_State;

namespace Solarus {

class SpriteAnimation;
class SpriteAnimationData;
class Tileset;

/**
 * \brief A set of animations representing a sprite.
 *
 * Each animation correspond to a specific situation of a sprite
 * and is an instance of SpriteAnimation.
 * For example, an NPC usually has an animation "stopped"
 * and an animation "walking".
 */
class SpriteAnimationSet {

  public:

    SpriteAnimationSet(const std::string& id);

    void set_tileset(Tileset& tileset);

    bool has_animation(const std::string& animation_name) const;
    const SpriteAnimation& get_animation(const std::string& animation_name) const;
    SpriteAnimation& get_animation(const std::string& animation_name);
    const std::string& get_default_animation() const;

    void enable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;
    const Size& get_max_size() const;

  private:

    void load();

    void add_animation(const std::string& animation_name,
        const SpriteAnimationData& animation_data);

    std::string id;                          /**< Id of this animation set. */
    std::map<std::string, SpriteAnimation>
            animations;                      /**< The animations */
    std::string default_animation_name;      /**< Name of the default animation. */
    Size max_size;                           /**< Size of this biggest frame. */

};

}

#endif

