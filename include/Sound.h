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
#ifndef ZSDX_SOUND_H
#define ZSDX_SOUND_H

#include "Common.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <list>

/**
 * This class represents a sound effet that can be played in ZSDX.
 * This class also handles the initialization of the whole sound system.
 * To create a sound, prefer the ResourceManager::get_sound() method
 * rather than calling directly the constructor of Sound.
 */
class Sound {

  private:

    ALuint buffer;             /**< the OpenAL buffer containing the PCM decoded data */
    std::list<ALuint> sources; /**< the sources currently playing this sound */

    static bool initialized;   /**< indicates that the audio system is initialized */
 
  public:

    Sound(const SoundId &sound_id);
    ~Sound(void);
    bool play(void);

    static void initialize(void);
    static void quit(void);
    static bool is_initialized(void);
    static void update(void);

};

#endif

