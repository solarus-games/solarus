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
#include <list>
#include <map>
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>

/**
 * @brief Represents a sound effet that can be played in the program.
 *
 * This class also handles the initialization of the whole audio system.
 * To create a sound, prefer the Sound::play() method
 * rather than calling directly the constructor of Sound.
 * This class is the only one that depends on the sound decoding library (libsndfile).
 * This class and the Music class are the only ones that depend on the audio mixer library (OpenAL).
 */
class Sound {

  private:

    static ALCdevice* device;
    static ALCcontext* context;

    SoundId id;							/**< id of this sound */
    ALuint buffer;						/**< the OpenAL buffer containing the PCM decoded data of this sound */
    std::list<ALuint> sources;					/**< the sources currently playing this sound */
    static std::list<Sound*> current_sounds;			/**< the sounds currently playing */
    static std::map<SoundId,Sound> all_sounds;			/**< all sounds created before */

    static bool initialized;					/**< indicates that the audio system is initialized */
    static float volume;					/**< the volume of sound effects (0.0 to 1.0) */

    ALuint decode_file(const std::string &file_name);
    bool update_playing();

  public:

    // libsndfile

    /**
     * @brief Buffer containing an encoded sound file.
     */
    struct SoundFromMemory {
      char *data;               /**< the buffer */
      size_t size;              /**< size of the buffer in bytes */
      size_t position;          /**< current position in the buffer */
    };

    static SF_VIRTUAL_IO sf_virtual;				/**< libsndfile object used to load the encoded sound from memory */

    // functions to load the encoded sound from memory
    static sf_count_t sf_get_filelen(void *user_data);
    static sf_count_t sf_seek(sf_count_t offset, int whence, void *user_data);
    static sf_count_t sf_read(void *ptr, sf_count_t count, void *user_data);
    static sf_count_t sf_write(const void *ptr, sf_count_t count, void *user_data);
    static sf_count_t sf_tell(void *user_data);

    Sound();
    Sound(const SoundId &sound_id);
    ~Sound();
    bool start();

    static void play(const SoundId &sound_id);

    static void initialize(int argc, char **argv);
    static void quit();
    static bool is_initialized();
    static void update();

    static int get_volume();
    static void set_volume(int volume);
};

#endif

