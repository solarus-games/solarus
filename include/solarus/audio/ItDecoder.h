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
#ifndef SOLARUS_IT_DECODER_H
#define SOLARUS_IT_DECODER_H

#include "solarus/core/Common.h"
#include <cstddef>
#include <memory>
#include <string>
#include <modplug.h>

namespace Solarus {

/**
 * \brief Encapsulates the Impulse Tracker music decoding.
 *
 * This class allows the Music class to be independent of the Impulse Tracker decoding library.
 */
class ItDecoder {

  public:

    ItDecoder();

    void load(const std::string& sound_buffer);
    void unload();
    int decode(void* decoded_data, int nb_samples);

    int get_num_channels() const;
    int get_channel_volume(int channel) const;
    void set_channel_volume(int channel, int volume);
    int get_tempo() const;
    void set_tempo(int tempo);
    bool loops() const;
    void set_loops(bool loops);

  private:

    struct ModPlugFileDeleter {
      void operator()(ModPlugFile* modplug_file) {
        ModPlug_Unload(modplug_file);
      }
    };
    using ModPlugFileUniquePtr = std::unique_ptr<ModPlugFile, ModPlugFileDeleter>;

    ModPlugFileUniquePtr modplug_file;

};

}

#endif

