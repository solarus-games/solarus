/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/audio/Sound.h"
#include "solarus/lua/ScopedLuaRef.h"
#include <memory>
#include <string>
#include <vector>

namespace Solarus {

class ItDecoder;
class OggDecoder;
class SpcDecoder;

/**
 * \brief Represents a music that can be played.
 *
 * A music should be in format .spc (Snes), .it (Impulse Tracker module) or .ogg.
 * The .mp3 format will probably be supported in a future version.
 * Only one music can be played at the same time.
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 * Sound and Music are the only classes that depends on audio libraries.
 *
 * TODO move the non-static parts to an internal private class.
 * TODO make a subclass for each format?
 */
class SOLARUS_API Music {

  public:

    /**
     * The music file formats recognized.
     */
    enum Format {
      NO_FORMAT,        /**< No music. */
      SPC,              /**< Original Snes music. */
      IT,               /**< Impulse Tracker module. */
      OGG               /**< Ogg Vorbis. */
    };

    static const std::string none;               /**< special id indicating that there is no music */
    static const std::string unchanged;          /**< special id indicating that the music is the same as before */
    static const std::vector<std::string>
        format_names;                            /**< Name of each format. */

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
    static void play(
        const std::string& music_id,
        bool loop
    );
    static void play(
        const std::string& music_id,
        bool loop,
        const ScopedLuaRef& callback_ref
    );
    static void stop_playing();
    static const std::string& get_current_music_id();

  private:

    Music();
    Music(
        const std::string& music_id,
        bool loop,
        const ScopedLuaRef& callback_ref
    );

    bool start();
    void stop();
    bool is_paused();
    void set_paused(bool pause);
    void set_callback(const ScopedLuaRef& callback_ref);

    void decode_spc(ALuint destination_buffer, ALsizei nb_samples);
    void decode_it(ALuint destination_buffer, ALsizei nb_samples);
    void decode_ogg(ALuint destination_buffer, ALsizei nb_samples);

    bool update_playing();

    std::string id;                              /**< id of this music */
    std::string file_name;                       /**< name of the file to play */
    Format format;                               /**< format of the music, detected from the file name */
    bool loop;                                   /**< Whether the music should loop. */
    ScopedLuaRef callback_ref;                   /**< Lua ref to a function to call when the music finishes. */

    static constexpr int nb_buffers = 8;
    ALuint buffers[nb_buffers];                  /**< multiple buffers used to stream the music */
    ALuint source;                               /**< the OpenAL source streaming the buffers */

    static std::unique_ptr<SpcDecoder>
        spc_decoder;                             /**< The SPC decoder. */
    static std::unique_ptr<ItDecoder>
        it_decoder;                              /**< The IT decoder. */
    static std::unique_ptr<OggDecoder>
        ogg_decoder;                             /**< The OGG decoder. */
    static float volume;                         /**< volume of musics (0.0 to 1.0) */

    static std::unique_ptr<Music> current_music; /**< the music currently played (if any) */

};

}

#endif

