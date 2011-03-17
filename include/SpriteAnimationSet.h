/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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

#include "Common.h"
#include <map>

/**
 * @brief A set of animations representing a sprite.
 *
 * Each animation correspond to a specific situation of a sprite
 * and is an instance of SpriteAnimation.
 * For example, an NPC usually has an animation "stopped"
 * and an animation "walking".
 */
class SpriteAnimationSet {

  private:

    std::map<std::string, SpriteAnimation*> animations;			/**< the animations */
    std::string default_animation_name;					/**< name of the default animation */

  public:

    SpriteAnimationSet();
    SpriteAnimationSet(const SpriteAnimationSetId &id);
    ~SpriteAnimationSet();

    void set_map(Map &map);

    bool has_animation(const std::string& animation_name) const;
    const SpriteAnimation *get_animation(const std::string &animation_name) const;
    SpriteAnimation *get_animation(const std::string &animation_name);
    const std::string& get_default_animation() const;

    void enable_pixel_collisions();
    bool are_pixel_collisions_enabled() const;
};

#endif

