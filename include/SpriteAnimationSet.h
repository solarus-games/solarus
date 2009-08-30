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
#ifndef ZSDX_SPRITE_ANIMATION_SET_H
#define ZSDX_SPRITE_ANIMATION_SET_H

#include "Common.h"
#include <map>

/**
 * A set of animations.
 * Each animation correspond to a specific situation
 * of a sprite.
 * For example, you can have an animation "stopped"
 * and an animation "walking".
 */
class SpriteAnimationSet {

 private:

  std::map<std::string, SpriteAnimation*> animations; /**< the animations */
  std::string default_animation_name;                 /**< name of the default animation */
  
 public:

  SpriteAnimationSet(const SpriteAnimationSetId &id);
  virtual ~SpriteAnimationSet(void);

  void set_map(Map *map);

  SpriteAnimation *get_animation(const std::string &animation_name);
  const std::string& get_default_animation(void);

  void enable_pixel_collisions(void);
  bool are_pixel_collisions_enabled(void);
};

#endif

