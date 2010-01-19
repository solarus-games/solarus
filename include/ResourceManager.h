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
#ifndef ZSDX_RESOURCE_MANAGER_H
#define ZSDX_RESOURCE_MANAGER_H

#include "Common.h"
#include <map>

/**
 * This class provides access to all external resources:
 * images, sounds, sprite animations...
 * Except for the images, all resources are stored into
 * memory when you get them for the first time and you
 * don't have to delete them. They are deleted when the application
 * ends. This does not consumes much memory because:
 * - the maps and tilesets have an unload() method you can call,
 * - the musics and sounds close their audio files when they stop.
 * The images are the only kind of resources that you have to
 * free yourself.
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

    static Surface * load_image(const ImageId &id);
    static Surface * load_image(const ImageId &id, bool relative_to_sprites_dir);
    // TODO flags for options relative_to_sprites_dir and language_specific - but do this directly in Surface

    static Tileset* get_tileset(TilesetId id);
    static Map* get_map(MapId id);
    static Music* get_music(const MusicId &id);
    static Sound* get_sound(const SoundId &id);
    static SpriteAnimationSet* get_sprite_animation_set(const SpriteAnimationSetId &id);
};

#endif

