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
#ifndef SOLARUS_OGG_DECODER_H
#define SOLARUS_OGG_DECODER_H

#include "solarus/core/Common.h"
#include "solarus/audio/Sound.h"
#include <memory>
#include <string>

namespace Solarus {

/**
 * \brief Encapsulates the Ogg Vorbis music decoding.
 */
class OggDecoder {

  public:

    OggDecoder();

    bool load(std::string&& ogg_data, bool loop);
    void unload();
    void decode(ALuint destination_buffer, ALsizei nb_samples);

  private:

    struct OggFileDeleter {
      void operator()(OggVorbis_File* ogg_file) {
        ov_clear(ogg_file);
        delete ogg_file;
      }
    };
    using OggFileUniquePtr = std::unique_ptr<OggVorbis_File, OggFileDeleter>;

    OggFileUniquePtr ogg_file;         /**< The file used by the vorbisfile lib. */
    Sound::SoundFromMemory ogg_mem;    /**< The encoded music loaded in memory,
                                        * passed to the vorbisfile lib as user data. */
    vorbis_info* ogg_info;             /**< Info about the OGG file. */
    ogg_int64_t loop_start_pcm;        /**< Where to loop to in PCM samples.
                                        * -1 means no loop. */
    ogg_int64_t loop_end_pcm;          /**< Where to loop from in PCM samples.
                                        * -1 means no loop. */

};

}

#endif

