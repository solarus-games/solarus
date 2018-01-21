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
#include "solarus/core/Debug.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/audio/OggDecoder.h"
#include <al.h>
#include <sstream>
#include <vector>

namespace Solarus {

/**
 * \brief Creates an Ogg decoder.
 */
OggDecoder::OggDecoder():
  ogg_file(),
  ogg_mem(),
  ogg_info(nullptr),
  loop_start_pcm(-1),
  loop_end_pcm(-1) {

}

/**
 * \brief Loads an OGG file from the memory.
 * \param ogg_data The memory area to read.
 * \param loop Whether the music should loop if reaching the end.
 * \return \c true in case of success.
 */
bool OggDecoder::load(std::string&& ogg_data, bool loop) {

  ogg_file = OggFileUniquePtr(new OggVorbis_File());

  ogg_mem.position = 0;
  ogg_mem.loop = loop;
  ogg_mem.data = ogg_data;
  // Now, ogg_mem contains the encoded data.

  int error = ov_open_callbacks(&ogg_mem, ogg_file.get(), nullptr, 0, Sound::ogg_callbacks);

  if (error != 0) {
    return false;
  }

  ogg_info = ov_info(ogg_file.get(), -1);

  // Read loop info from the header.
  vorbis_comment* header = ov_comment(ogg_file.get(), -1);
  if (header == nullptr) {
    return false;
  }

  const char* loop_start_string = vorbis_comment_query(header, "LOOPSTART", 0);
  const char* loop_end_string = vorbis_comment_query(header, "LOOPEND", 0);
  const char* loop_length_string = vorbis_comment_query(header, "LOOPLENGTH", 0);

  if (loop_start_string != nullptr) {
    std::istringstream iss(loop_start_string);
    ogg_int64_t pcm = -1;
    if (iss >> pcm && pcm >= 0) {
      // There is a loop.
      loop_start_pcm = pcm;
      loop_end_pcm = ov_pcm_total(ogg_file.get(), -1);  // Default loop end value.
    }

    if (loop_end_string != nullptr) {
      iss.clear();
      iss.str(loop_end_string);
      if (iss >> pcm && pcm > 0) {
        // There is a loop end info.
        loop_end_pcm = pcm;
      }
    }
    else if (loop_length_string != nullptr) {
      iss.clear();
      iss.str(loop_length_string);
      if (iss >> pcm && pcm > 0) {
        // There is a loop end info from a length value.
        loop_end_pcm = loop_start_pcm + pcm;
      }
    }
  }

  return true;
}

/**
 * \brief Unloads the OGG data from memory.
 */
void OggDecoder::unload() {
  ogg_file = nullptr;
  ogg_mem.data.clear();
  ogg_info = nullptr;
  loop_start_pcm = -1;
  loop_end_pcm = -1;
}

/**
 * \brief Decodes a chunk of the previously loaded OGG data into PCM data
 * and plays it.
 * \param decoded_data Pointer to where you want the decoded data to be written.
 * \param nb_samples Number of samples to write.
 */
void OggDecoder::decode(ALuint destination_buffer, ALsizei nb_samples) {

  if (ogg_info == nullptr) {
    return;
  }

  // Read the encoded music properties.
  const int num_channels = ogg_info->channels;
  ALsizei sample_rate = ALsizei(ogg_info->rate);
  ALenum al_format = AL_NONE;
  if (num_channels == 1) {
    al_format = AL_FORMAT_MONO16;
  }
  else if (num_channels == 2) {
    al_format = AL_FORMAT_STEREO16;
  }

  const ogg_int64_t loop_end_byte = loop_end_pcm * num_channels * sizeof(ALshort);

  // Decode the OGG data.
  std::vector<ALshort> raw_data(nb_samples * num_channels);
  int bitstream = 0;
  long bytes_read = 0;
  long total_bytes_read = 0;
  long remaining_bytes = nb_samples * num_channels * sizeof(ALshort);

  do {
    long max_bytes_to_read = remaining_bytes;
    ogg_int64_t current_pcm = ov_pcm_tell(ogg_file.get());
    ogg_int64_t current_byte = current_pcm * num_channels * sizeof(ALshort);

    if (loop_end_pcm != -1 &&
        current_byte + max_bytes_to_read > loop_end_byte) {
      // Don't read after the loop point.
      max_bytes_to_read = (long) loop_end_byte - current_byte;
    }

    bytes_read = ov_read(
        ogg_file.get(),
        ((char*) raw_data.data()) + total_bytes_read,
        max_bytes_to_read,
        0,
        2,
        1,
        &bitstream
    );

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

      // Check if we should loop now.
      current_pcm = ov_pcm_tell(ogg_file.get());

      if (loop_end_pcm != -1 &&
          loop_start_pcm != -1 &&
          current_pcm == loop_end_pcm) {
        int error = ov_pcm_seek(ogg_file.get(), loop_start_pcm);
        if (error != 0) {
          std::ostringstream oss;
          oss << "Failed to loop in OGG file: error " << error;
          Debug::error(oss.str());
        }
      }
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

