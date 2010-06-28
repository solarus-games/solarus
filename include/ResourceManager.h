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
 * @brief Provides storage and access to exteranl resources.
 *
 * This class provides storage and access to some external resources:
 * maps, sounds, sprite animations, etc.
 * Those resources are stored into
 * memory when you get them for the first time and you
 * don't have to delete them. They are deleted when the program ends.
 * This does not consumes much memory because:
 * - the maps and tilesets have an unload() method you can call,
 * - the musics and sounds close their audio files when they stop.
 */
class ResourceManager {

  private:

    static ResourceManager *instance;

    std::map<TilesetId, Tileset*> tilesets;
    std::map<MapId, Map*> maps;
    std::map<MusicId, Music*> musics;
    std::map<SoundId, Sound*> sounds;
    std::map<SpriteAnimationSetId, SpriteAnimationSet*> sprite_animations;

    ResourceManager(void);
    ~ResourceManager(void);

  public:

    static void initialize(void);
    static void quit(void);

    static Tileset* get_tileset(TilesetId id);
    static Map* get_map(MapId id);
    static Music* get_music(const MusicId &id);
    static Sound* get_sound(const SoundId &id);
    static SpriteAnimationSet* get_sprite_animation_set(const SpriteAnimationSetId &id);
};

#endif

