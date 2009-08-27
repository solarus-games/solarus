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
#include "Sound.h"

/**
 * This class represents a music that can be played.
 * A music should be in format .spc (Snes) .it (Impulse Tracker Module).
 * Only one music can be played at the same time.
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 * Music and Sound should be the only modules that depends on an audio library.
 */
class Music: public Sound {

  private:

    /**
     * The music file formats recognized.
     */
    enum Format {
      SPC, /**< Snes */
      IT,  /**< Impulse Tracker module */
    };

    Format format;    /**< format of the music, detected based on the file name */

    char *spc_data;  /**< the SPC music (only for SPC format) */
    Mix_Music *music; /**< the music object (only for Impulse Tracker format) */

    bool play_spc(void);
    bool play_it(void);

  public:

    static const MusicId none;
    static const MusicId unchanged;

    Music(const MusicId &music_id);
    ~Music(void);

    bool play(void);
    void stop(void);
    bool is_paused(void);
    void set_paused(bool pause);

    static void spc_player(void *spc_data, Uint8 *raw_data, int length);

    static bool isNoneId(const MusicId &music_id);
    static bool isUnchangedId(const MusicId &music_id);
    static bool isEqualId(const MusicId &music_id, const MusicId &other_music_id);
};

#endif

