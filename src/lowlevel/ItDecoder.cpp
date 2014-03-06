/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include <modplug.h>
#include <stdafx.h>  // These two headers are with the libmodplug ones.
#include <sndfile.h>

namespace solarus {

/**
 * \brief Creates an Impulse Tracker decoder.
 */
ItDecoder::ItDecoder():
  modplug_file(NULL) {

  ModPlug_Settings settings;
  ModPlug_GetSettings(&settings);
  settings.mChannels = 2;     // stereo
  settings.mBits = 16;        // 16 bits
  settings.mLoopCount = -1;   // Loop forever by default.
  ModPlug_SetSettings(&settings);
}

/**
 * \brief Destructor.
 */
ItDecoder::~ItDecoder() {
}

/**
 * \brief Loads an IT file from memory.
 * \param sound_data the memory area to read
 * \param sound_size size of the memory area in bytes
 */
void ItDecoder::load(void* sound_data, size_t sound_size) {

  Debug::check_assertion(modplug_file == NULL,
      "IT data is already loaded"
  );

  // Load the IT data into the IT library.
  modplug_file = ModPlug_Load((const void*) sound_data, int(sound_size));
}

/**
 * \brief Unloads the IT previously loaded.
 */
void ItDecoder::unload() {

  Debug::check_assertion(modplug_file != NULL,
      "IT data is not loaded"
  );

  ModPlug_Unload(modplug_file);
  modplug_file = NULL;
}

/**
 * \brief Decodes a chunk of the previously loaded IT data into PCM data.
 * \param decoded_data Pointer to where you want the decoded data to be written.
 * \param nb_samples Number of samples to write.
 */
void ItDecoder::decode(void* decoded_data, int nb_samples) {

  // Decode from the IT data the specified number of PCM samples.
  ModPlug_Read(modplug_file, decoded_data, nb_samples);
}

/**
 * \brief Returns the number of channels in this music.
 * \return The number of channels.
 */
int ItDecoder::get_num_channels() const {
  return ModPlug_NumChannels(modplug_file);
}

/**
 * \brief Returns the volume of a channel.
 * \param channel A channel index.
 * \return The volume of this channel.
 */
int ItDecoder::get_channel_volume(int channel) const {

  const int num_patterns = ModPlug_NumPatterns(modplug_file);

  Debug::check_assertion(channel >= 0 && channel < get_num_channels(),
      "Invalid channel number");

  if (num_patterns == 0) {
    return 0;
  }

  unsigned int num_rows = 0;
  ModPlugNote* notes = ModPlug_GetPattern(modplug_file, 0, &num_rows);

  if (num_rows == 0) {
    return 0;
  }

  return notes[0].Volume;
}

/**
 * \brief Sets the volume of a channel.
 * \param channel A channel index.
 * \param volume The volume to set.
 */
void ItDecoder::set_channel_volume(int channel, int volume) {

  const unsigned int num_channels = get_num_channels();
  const unsigned int num_patterns = ModPlug_NumPatterns(modplug_file);

  for (unsigned int pattern = 0; pattern < num_patterns; ++pattern) {
    unsigned int num_rows;
    ModPlugNote* notes = ModPlug_GetPattern(modplug_file, pattern, &num_rows);
    for (unsigned int j = channel; j < num_rows * num_channels; j += num_channels) {
      notes[j].Volume = volume;
    }
  }
}

/**
 * \brief Returns the tempo of the music.
 * \return The tempo.
 */
int ItDecoder::get_tempo() const {

  return reinterpret_cast<CSoundFile*>(modplug_file)->GetMusicTempo();
}

/**
 * \brief Sets the tempo of the music.
 * \param tempo The tempo to set.
 */
void ItDecoder::set_tempo(int tempo) {

  reinterpret_cast<CSoundFile*>(modplug_file)->SetTempo(tempo);
}

/**
 * \brief Returns whether the decoder loops when reaching the end.
 */
bool ItDecoder::loops() const {

  ModPlug_Settings settings;
  ModPlug_GetSettings(&settings);
  return settings.mLoopCount == -1;  // -1 means looping forever.
}

/**
 * \brief Sets whether the decoder should loop when reaching the end.
 * \param loops \c true to make the decoder loop.
 */
void ItDecoder::set_loops(bool loops) {

  ModPlug_Settings settings;
  ModPlug_GetSettings(&settings);
  settings.mLoopCount = loops ? -1 : 0;
  ModPlug_SetSettings(&settings);
}

}

