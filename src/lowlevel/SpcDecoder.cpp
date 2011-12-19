/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lowlevel/SpcDecoder.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates an SPC decoder.
 */
SpcDecoder::SpcDecoder() {

  // initialize the SPC library
  snes_spc_manager = spc_new();
  snes_spc_filter = spc_filter_new();
}

/**
 * @brief Destructor.
 */
SpcDecoder::~SpcDecoder() {

  // uninitialize the SPC library
  spc_filter_delete(snes_spc_filter);
  spc_delete(snes_spc_manager);
}

/**
 * @brief Loads an SPC file from the memory.
 * @param sound_data the memory area to read
 * @param sound_size size of the memory area in bytes
 */
void SpcDecoder::load(int16_t *sound_data, size_t sound_size) {

  // load the SPC data into the SPC library
  spc_load_spc(snes_spc_manager, (short int*) sound_data, sound_size);
  spc_clear_echo(snes_spc_manager);
  spc_filter_clear(snes_spc_filter);
}


/**
 * @brief Decodes a chunk of the previously loaded SPC data into PCM data.
 * @param decoded_data pointer to where you want the decoded data to be wrote
 * @param nb_samples number of samples to write
 */
void SpcDecoder::decode(int16_t *decoded_data, int nb_samples) {

  // decode from the SPC data the specified number of PCM samples

  const char *err = spc_play(snes_spc_manager, nb_samples, (short int*) decoded_data);
  Debug::check_assertion(err == NULL, StringConcat() << "Failed to decode SPC data: " << err);
  spc_filter_run(snes_spc_filter, (short int*) decoded_data, nb_samples);
}

