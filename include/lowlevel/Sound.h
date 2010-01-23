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
#ifndef SOLARUS_SOUND_H
#define SOLARUS_SOUND_H

#include "Common.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <list>
#include <sndfile.h>

/**
 * This class represents a sound effet that can be played in the program.
 * This class also handles the initialization of the whole audio system.
 * To create a sound, prefer the ResourceManager::get_sound() method
 * rather than calling directly the constructor of Sound.
 * This class is the only one that calls the sound decoding library (sndfile).
 * This class and the Music class are the only ones that call the audio mixer library (OpenAL).
 */
class Sound {

  private:

    static ALCdevice* device;
    static ALCcontext* context;

    ALuint buffer;                           /**< the OpenAL buffer containing the PCM decoded data of this sound */
    std::list<ALuint> sources;               /**< the sources currently playing this sound */
    static std::list<Sound*> current_sounds; /**< the sounds currently playing */

    static bool initialized;                 /**< indicates that the audio system is initialized */
 
    struct WavFromMemory {                   /**< buffer containing a wav file */
      char *data;
      size_t size;
      size_t position;
    };

    static SF_VIRTUAL_IO sf_virtual;         /**< libsndfile object used to load the wav from memory */

    // functions to load wav from memory
    static sf_count_t sf_get_filelen(void *user_data);
    static sf_count_t sf_seek(sf_count_t offset, int whence, void *user_data);
    static sf_count_t sf_read(void *ptr, sf_count_t count, void *user_data);
    static sf_count_t sf_write(const void *ptr, sf_count_t count, void *user_data);
    static sf_count_t sf_tell(void *user_data);

    ALuint decode_wav(const std::string &file_name);
    bool update_playing(void);

  public:

    Sound(const SoundId &sound_id);
    ~Sound(void);
    bool play(void);

    static void initialize(int argc, char **argv);
    static void quit(void);
    static bool is_initialized(void);
    static void update(void);
};

#endif

