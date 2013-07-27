/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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

/**
 * \brief Represents a music that can be played.
 *
 * A music should be in format .spc (Snes), .it (Impulse Tracker module) or .ogg.
 * The .mp3 format will probably be supported in a future version.
 * Only one music can be played at the same time.
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 * Sound and Music are the only classes that depends on audio libraries.
 */
class Music { // TODO make a subclass for each format, or at least make a better separation between them

  public:

    /**
     * The music file formats recognized.
     */
    enum Format {
      NO_FORMAT,  /**< No music. */
      SPC,        /**< original Snes music */
      IT,         /**< Impulse Tracker module */
      OGG         /**< Ogg Vorbis */
    };

    static const std::string none;               /**< special id indicating that there is no music */
    static const std::string unchanged;          /**< special id indicating that the music is the same as before */
    static const std::string format_names[];     /**< Name of each format. */

    Music(const std::string& music_id = none);
    ~Music();

    static void initialize();
    static void quit();
    static bool is_initialized();
    static void update();

    static Format get_format();
    static int get_volume();
    static void set_volume(int volume);
    static int get_num_channels();
    static int get_channel_volume(int channel);
    static void set_channel_volume(int channel, int volume);
    static int get_tempo();
    static void set_tempo(int tempo);

    static void find_music_file(const std::string& music_id,
        std::string& file_name, Format& format);
    static bool exists(const std::string& music_id);
    static void play(const std::string& music_id);
    static Music* get_current_music();
    static const std::string& get_current_music_id();

  private:

    bool start();
    void stop();
    bool is_paused();
    void set_paused(bool pause);

    void decode_spc(ALuint destination_buffer, ALsizei nb_samples);
    void decode_it(ALuint destination_buffer, ALsizei nb_samples);
    void decode_ogg(ALuint destination_buffer, ALsizei nb_samples);

    void update_playing();

    std::string id;                              /**< id of this music */
    std::string file_name;                       /**< name of the file to play */
    Format format;                               /**< format of the music, detected from the file name */

    // OGG specific
    OggVorbis_File ogg_file;                     /**< the file used by the vorbisfile lib */
    Sound::SoundFromMemory ogg_mem;              /**< the encoded music loaded in memory, passed to the vorbisfile lib as user data */

    static const int nb_buffers = 8;
    ALuint buffers[nb_buffers];                  /**< multiple buffers used to stream the music */
    ALuint source;                               /**< the OpenAL source streaming the buffers */

    static SpcDecoder* spc_decoder;              /**< the SPC decoder */
    static ItDecoder* it_decoder;                /**< the IT decoder */
    static float volume;                         /**< volume of musics (0.0 to 1.0) */

    static Music* current_music;                 /**< the music currently played (if any) */
    static std::map<std::string, Music> all_musics;   /**< all musics created before */

};

#endif

