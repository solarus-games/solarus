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
#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include "Common.h"
#include <AL/al.h>

/**
 * This class represents a music that can be played.
 * A music should be in format .spc (Snes) or .it (Impulse Tracker Module).
 * Only one music can be played at the same time.
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 * Sound and Music should be the only modules that depends on an audio mixer library.
 */
class Music {

  private:

    /**
     * The music file formats recognized.
     */
    enum Format {
      SPC, /**< Snes */
      IT,  /**< Impulse Tracker module (TODO implement with the modplug lib) */
    };

    std::string file_name;          /**< name of the file to play */
    Format format;                  /**< format of the music, detected from the file name */

    static const int nb_buffers = 2;
    ALuint buffers[nb_buffers];     /**< multiple buffers used to stream the music */
    ALuint source;                  /**< the OpenAL source streaming the buffers */

    static Music *current_music;    /**< the music currently played (if any) */
    static SpcDecoder *spc_decoder;  /**< the SPC decoder */

  public:

    static const MusicId none;      /**< special id indicating that there is no music */
    static const MusicId unchanged; /**< special id indicating that the music is the same as before */

    Music(const MusicId &music_id);
    ~Music(void);

    static void initialize(void);
    static void quit(void);
    static bool is_initialized(void);
    static void update(void);

    bool play(void);
    void stop(void);
    void update_playing(void);
    bool is_paused(void);
    void set_paused(bool pause);

    void decode_spc(ALuint destination_buffer, ALsizei nb_samples);

    static bool isNoneId(const MusicId &music_id);
    static bool isUnchangedId(const MusicId &music_id);
    static bool isEqualId(const MusicId &music_id, const MusicId &other_music_id);
};

#endif

