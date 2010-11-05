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
#include "lowlevel/Sound.h"
#include <map>
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
class Music { // TODO make a subclass for each format, or at least make a better separation between them

  private:

    /**
     * The music file formats recognized.
     */
    enum Format {
      SPC,	/**< Snes */
      OGG,	/**< Ogg Vorbis */
      IT	/**< Impulse Tracker module (TODO implement with the modplug lib) */
    };

    MusicId id;							/**< id of this music */
    std::string file_name;					/**< name of the file to play */
    Format format;						/**< format of the music, detected from the file name */

    // OGG specific
    SNDFILE *sf_file;						/**< the file used by libsndfile */
    SF_INFO sf_file_info;					/**< properties of the sound file */
    Sound::SoundFromMemory sf_mem;				/**< the encoded music loaded in memory, passed to libsndfile as user data */

    static const int nb_buffers = 8;
    ALuint buffers[nb_buffers];					/**< multiple buffers used to stream the music */
    ALuint source;						/**< the OpenAL source streaming the buffers */

    static SpcDecoder *spc_decoder;				/**< the SPC decoder */
    static float volume;					/**< volume of musics (0.0 to 1.0) */

    static Music *current_music;				/**< the music currently played (if any) */
    static std::map<MusicId,Music> all_musics;			/**< all musics created before */

    void update_playing();

  public:

    static const MusicId none;					/**< special id indicating that there is no music */
    static const MusicId unchanged;				/**< special id indicating that the music is the same as before */

    Music();
    Music(const MusicId &music_id);
    ~Music();

    static void initialize();
    static void quit();
    static bool is_initialized();
    static void update();

    static int get_volume();
    static void set_volume(int volume);

    static void play(const MusicId &music_id);
    static Music* get_current_music();
    static const MusicId& get_current_music_id();

    bool start();
    void stop();
    bool is_paused();
    void set_paused(bool pause);

    void decode_spc(ALuint destination_buffer, ALsizei nb_samples);
    void decode_ogg(ALuint destination_buffer, ALsizei nb_samples);
};

#endif

