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
#include "lowlevel/ItDecoder.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates an Impulse Tracker decoder.
 */
ItDecoder::ItDecoder():
  modplug_file(NULL) {

  ModPlug_Settings settings;
  ModPlug_GetSettings(&settings);
  settings.mChannels = 2;     // stereo
  settings.mBits = 16;        // 16 bits
  settings.mLoopCount = -1;   // loop forever
  ModPlug_SetSettings(&settings);
}

/**
 * @brief Destructor.
 */
ItDecoder::~ItDecoder() {
}

/**
 * @brief Loads an IT file from memory.
 * @param sound_data the memory area to read
 * @param sound_size size of the memory area in bytes
 */
void ItDecoder::load(void* sound_data, size_t sound_size) {

  // load the IT data into the IT library
  modplug_file = ModPlug_Load((const void*) sound_data, sound_size);
}

/**
 * @brief Unloads the IT previously loaded.
 */
void ItDecoder::unload() {

  ModPlug_Unload(modplug_file);
}

/**
 * @brief Decodes a chunk of the previously loaded SPC data into PCM data.
 * @param decoded_data pointer to where you want the decoded data to be written
 * @param nb_samples number of samples to write
 */
void ItDecoder::decode(void* decoded_data, int nb_samples) {

  // decode from the IT data the specified number of PCM samples
  ModPlug_Read(modplug_file, decoded_data, nb_samples);
}

