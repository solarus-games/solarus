/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/OggDecoder.h"
#include "solarus/lowlevel/QuestFiles.h"
#include <AL/al.h>
#include <sstream>
#include <vector>

namespace Solarus {

/**
 * \brief Creates an Ogg decoder.
 */
OggDecoder::OggDecoder() {

}

/**
 * \brief Loads an OGG file from the memory.
 * \param ogg_data The memory area to read.
 * \param loop Whether the music should loop if reaching the end.
 * \return \c true in case of success.
 */
bool OggDecoder::load(const std::string& ogg_data, bool loop) {

  ogg_file = OggFileUniquePtr(new OggVorbis_File());

  ogg_mem.position = 0;
  ogg_mem.loop = loop;
  ogg_mem.data = ogg_data;
  // Now, ogg_mem contains the encoded data.

  int error = ov_open_callbacks(&ogg_mem, ogg_file.get(), nullptr, 0, Sound::ogg_callbacks);
  return error == 0;
}

/**
 * \brief Unloads the OGG data from memory.
 */
void OggDecoder::unload() {
  ogg_file = nullptr;
  ogg_mem.data.clear();
}

/**
 * \brief Decodes a chunk of the previously loaded OGG data into PCM data
 * and plays it.
 * \param decoded_data Pointer to where you want the decoded data to be written.
 * \param nb_samples Number of samples to write.
 * \return The number of bytes read, or 0 if the end is reached.
 */
void OggDecoder::decode(ALuint destination_buffer, ALsizei nb_samples) {

  // Read the encoded music properties.
  vorbis_info* info = ov_info(ogg_file.get(), -1);
  ALsizei sample_rate = ALsizei(info->rate);

  ALenum al_format = AL_NONE;
  if (info->channels == 1) {
    al_format = AL_FORMAT_MONO16;
  }
  else if (info->channels == 2) {
    al_format = AL_FORMAT_STEREO16;
  }

  // Decode the OGG data.
  std::vector<ALshort> raw_data(nb_samples * info->channels);
  int bitstream;
  long bytes_read;
  long total_bytes_read = 0;
  long remaining_bytes = nb_samples * info->channels * sizeof(ALshort);
  do {
    bytes_read = ov_read(ogg_file.get(), ((char*) raw_data.data()) + total_bytes_read, int(remaining_bytes), 0, 2, 1, &bitstream);
    if (bytes_read < 0) {
      if (bytes_read != OV_HOLE) { // OV_HOLE is normal when the music loops
        std::ostringstream oss;
        oss << "Error while decoding ogg chunk: " << bytes_read;
        Debug::error(oss.str());
        return;
      }
    }
    else {
      total_bytes_read += bytes_read;
      remaining_bytes -= bytes_read;
    }
  }
  while (remaining_bytes > 0 && bytes_read > 0);

  // Put this decoded data into the buffer.
  alBufferData(destination_buffer, al_format, raw_data.data(), ALsizei(total_bytes_read), sample_rate);

  int error = alGetError();
  if (error != AL_NO_ERROR) {
    std::ostringstream oss;
    oss << "Failed to fill the audio buffer with decoded OGG data: error " << error;
    Debug::error(oss.str());
  }
}

}

