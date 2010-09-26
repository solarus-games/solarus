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
#ifndef SOLARUS_RESOURCE_MANAGER_H
#define SOLARUS_RESOURCE_MANAGER_H

#include "Common.h"
#include <map>

/**
 * @brief Provides storage and access to some external resources.
 *
 * This class provides storage and access to some external resources.
 * Those resources are stored into
 * memory when you get them for the first time and you
 * don't have to delete them. They are deleted when the program ends.
 * This does not consumes much memory because the tilesets have an unload() method you can call.
 *
 * Note: this class is not very useful and will probably become obsolete soon.
 */
class ResourceManager {

  private:

    static ResourceManager *instance;

    std::map<TilesetId, Tileset*> tilesets;
    std::map<SpriteAnimationSetId, SpriteAnimationSet*> sprite_animations;

    ResourceManager();
    ~ResourceManager();

  public:

    static void initialize();
    static void quit();

    static Tileset* get_tileset(TilesetId id);
    static SpriteAnimationSet* get_sprite_animation_set(const SpriteAnimationSetId &id);
};

#endif

