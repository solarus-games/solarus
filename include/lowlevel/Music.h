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
#ifndef SOLARUS_MUSIC_H
#define SOLARUS_MUSIC_H

#include "Common.h"
#include <AL/al.h>

/**
 * @brief Represents a music that can be played.
 *
 * A music should be in format .spc (Snes).
 * The .it format (Impulse Tracker Module) will probably be supported in a future version.
 * Only one music can be played at the same time.
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 * Sound and Music are the only classes that depends on audio libraries.
 */
class Music {

  private:

    /**
     * The music file formats recognized.
     */
    enum Format {
      SPC, /**< Snes */
      IT   /**< Impulse Tracker module (TODO implement with the modplug lib) */
    };

    std::string file_name;           /**< name of the file to play */
    Format format;                   /**< format of the music, detected from the file name */

    static const int nb_buffers = 8;
    ALuint buffers[nb_buffers];      /**< multiple buffers used to stream the music */
    ALuint source;                   /**< the OpenAL source streaming the buffers */

    static Music *current_music;     /**< the music currently played (if any) */
    static SpcDecoder *spc_decoder;  /**< the SPC decoder */
    static float volume;             /**< volume of musics (0.0 to 1.0) */

    void update_playing();

  public:

    static const MusicId none;       /**< special id indicating that there is no music */
    static const MusicId unchanged;  /**< special id indicating that the music is the same as before */

    Music(const MusicId &music_id);
    ~Music();

    static void initialize();
    static void quit();
    static bool is_initialized();
    static void update();

    static int get_volume();
    static void set_volume(int volume);

    static Music* get_current_music();

    bool play();
    void stop();
    bool is_paused();
    void set_paused(bool pause);

    void decode_spc(ALuint destination_buffer, ALsizei nb_samples);

    static bool is_none_id(const MusicId &music_id);
    static bool is_unchanged_id(const MusicId &music_id);
    static bool is_equal_id(const MusicId &music_id, const MusicId &other_music_id);
};

#endif

