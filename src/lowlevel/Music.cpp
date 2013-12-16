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
#include "lowlevel/Music.h"
#include "lowlevel/SpcDecoder.h"
#include "lowlevel/ItDecoder.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <vector>

namespace solarus {

const int Music::nb_buffers;
SpcDecoder* Music::spc_decoder = NULL;
ItDecoder* Music::it_decoder = NULL;
float Music::volume = 1.0;
Music* Music::current_music = NULL;
std::map<std::string, Music> Music::all_musics;

const std::string Music::none = "none";
const std::string Music::unchanged = "same";

/**
 * \brief Lua name of each music format.
 */
const std::string Music::format_names[] = {
  "",
  "spc",
  "it",
  "ogg",
  ""  // Sentinel.
};

/**
 * \brief Creates a new music.
 * \param music_id id of the music (file name without extension)
 */
Music::Music(const std::string& music_id):
  id(music_id),
  format(OGG) {

  if (!is_initialized() || music_id == none) {
    return;
  }

  for (int i = 0; i < nb_buffers; i++) {
    buffers[i] = AL_NONE;
  }
  source = AL_NONE;
}

/**
 * \brief Destroys the music.
 */
Music::~Music() {

  if (!is_initialized()) {
    return;
  }

  if (current_music == this) {
    stop();
  }
}

/**
 * \brief Initializes the music system.
 */
void Music::initialize() {

  // initialize the decoding features
  spc_decoder = new SpcDecoder();
  it_decoder = new ItDecoder();

  set_volume(100);
}

/**
 * \brief Exits the music system.
 */
void Music::quit() {
  if (is_initialized()) {
    all_musics.clear();
    delete spc_decoder;
    delete it_decoder;
  }
}

/**
 * \brief Returns whether the music system is initialized.
 * \return true if the music system is initilialized
 */
bool Music::is_initialized() {
  return spc_decoder != NULL;
}

/**
 * \brief Returns the format of the current music.
 * \return The format of the current music.
 */
Music::Format Music::get_format() {

  if (current_music == NULL) {
    return NO_FORMAT;
  }

  return current_music->format;
}

/**
 * \brief Returns the current volume of music.
 * \return the volume (0 to 100)
 */
int Music::get_volume() {

  return (int) (volume * 100.0 + 0.5);
}

/**
 * \brief Sets the volume of musics.
 * \param volume the new volume (0 to 100)
 */
void Music::set_volume(int volume) {

  volume = std::min(100, std::max(0, volume));
  Music::volume = volume / 100.0;

  if (current_music != NULL) {
    alSourcef(current_music->source, AL_GAIN, Music::volume);
  }
}

/**
 * \brief Returns the number of channels of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \return The number of channels.
 */
int Music::get_num_channels() {

  Debug::check_assertion(get_format() == IT,
      "This function is only supported for .it musics");

  return it_decoder->get_num_channels();
}

/**
 * \brief Returns the volume of a channel of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param channel Index of a channel.
 * \return The volume of this channel.
 */
int Music::get_channel_volume(int channel) {

  Debug::check_assertion(get_format() == IT,
      "This function is only supported for .it musics");

  return it_decoder->get_channel_volume(channel);
}

/**
 * \brief Sets the volume of a channel of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param channel Index of a channel.
 * \param volume The volume to set.
 */
void Music::set_channel_volume(int channel, int volume) {

  Debug::check_assertion(get_format() == IT,
      "This function is only supported for .it musics");

  it_decoder->set_channel_volume(channel, volume);
}

/**
 * \brief Returns the tempo of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \return The tempo of the current music.
 */
int Music::get_tempo() {

  Debug::check_assertion(get_format() == IT,
      "This function is only supported for .it musics");

  return it_decoder->get_tempo();
}

/**
 * \brief Sets the tempo of the current music.
 *
 * This function is only supported for .it musics.
 *
 * \param tempo The tempo to set.
 */
void Music::set_tempo(int tempo) {

  Debug::check_assertion(get_format() == IT,
      "This function is only supported for .it musics");

  it_decoder->set_tempo(tempo);
}


/**
 * \brief Returns the music currently playing.
 * \return the current music, or NULL if no music is being played
 */
Music* Music::get_current_music() {
  return current_music;
}

/**
 * \brief Returns the id of the music currently playing.
 * \return the id of the current music, or "none" if no music is being played
 */
const std::string& Music::get_current_music_id() {
  return current_music != NULL ? current_music->id : none;
}

/**
 * \brief Tries to find a music file from a music id.
 * \param music_id Id of the music to find (file name without
 * directory or extension). Cannot be Music::none or Music::unchanged.
 * \param file_name Resulting file name with its extension
 * (empty string if not found).
 * \param format Resulting music format.
 */
void Music::find_music_file(const std::string& music_id,
    std::string& file_name, Format& format) {

  file_name = "";
  format = OGG;

  std::string file_name_start = std::string("musics/" + music_id);
  if (FileTools::data_file_exists(file_name_start + ".ogg")) {
    format = OGG;
    file_name = file_name_start + ".ogg";
  }
  else if (FileTools::data_file_exists(file_name_start + ".it")) {
    format = IT;
    file_name = file_name_start + ".it";
  }
  else if (FileTools::data_file_exists(file_name_start + ".spc")) {
    format = SPC;
    file_name = file_name_start + ".spc";
  }
}

/**
 * \brief Returns whether a music exists.
 * \param music_id Id of the music to test. Music::none and Music::unchanged
 * are also considered valid.
 * \return true If this music exists.
 */
bool Music::exists(const std::string& music_id) {

  if (music_id == none || music_id == unchanged) {
    return true;
  }

  std::string file_name;
  Format format;
  find_music_file(music_id, file_name, format);

  return !file_name.empty();
}

/**
 * \brief Plays a music.
 *
 * If the music is different from the current one,
 * the current one is stopped.
 * The music specified can also be Music::none_id (then the current music is just stopped)
 * or even Music::unchanged_id (nothing is done in this case).
 *
 * \param music_id id of the music to play (file name without extension)
 */
void Music::play(const std::string& music_id) {

  if (music_id != unchanged && music_id != get_current_music_id()) {
    // the music is changed

    if (music_id == none && current_music != NULL) {

      current_music->stop();
      current_music = NULL;
    }
    else {

      // play another music
      if (current_music != NULL) {
        current_music->stop();
      }

      if (all_musics.count(music_id) == 0) {
        all_musics[music_id] = Music(music_id);
      }

      Music& music = all_musics[music_id];
      if (music.start()) {
        current_music = &music;
      }
      else {
        current_music = NULL;
      }
    }
  }
}

/**
 * \brief Updates the music system.
 *
 * When a music is playing, this function makes it update.
 */
void Music::update() {

  if (!is_initialized()) {
    return;
  }

  if (current_music != NULL) {
    current_music->update_playing();
  }
}

/**
 * \brief Updates this music when it is playing.
 *
 * This function handles the double buffering.
 */
void Music::update_playing() {

  // get the empty buffers
  ALint nb_empty;
  alGetSourcei(source, AL_BUFFERS_PROCESSED, &nb_empty);

  // refill them
  for (int i = 0; i < nb_empty; i++) {
    ALuint buffer;
    alSourceUnqueueBuffers(source, 1, &buffer); // unqueue the buffer

    // fill it by decoding more data
    switch (format) {

      case SPC:
        decode_spc(buffer, 4096);
        break;

      case IT:
        decode_it(buffer, 4096);
        break;

      case OGG:
        decode_ogg(buffer, 4096);
        break;

      case NO_FORMAT:
        Debug::die("Invalid music format");
        break;
    }
    alSourceQueueBuffers(source, 1, &buffer);   // queue it again
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);

  if (status != AL_PLAYING) {
    alSourcePlay(source);
  }
}

/**
 * \brief Decodes a chunk of SPC data into PCM data for the current music.
 * \param destination_buffer the destination buffer to write
 * \param nb_samples number of samples to write
 */
void Music::decode_spc(ALuint destination_buffer, ALsizei nb_samples) {

  // decode the SPC data
  std::vector<ALushort> raw_data(nb_samples);
  spc_decoder->decode((int16_t*) &raw_data[0], nb_samples);

  // put this decoded data into the buffer
  alBufferData(destination_buffer, AL_FORMAT_STEREO16, &raw_data[0], nb_samples * 2, 32000);

  int error = alGetError();
  Debug::check_assertion(error == AL_NO_ERROR,
      StringConcat() << "Failed to fill the audio buffer with decoded SPC data for music file '" << file_name << ": error " << error);
}

/**
 * \brief Decodes a chunk of IT data into PCM data for the current music.
 * \param destination_buffer the destination buffer to write
 * \param nb_samples number of samples to write
 */
void Music::decode_it(ALuint destination_buffer, ALsizei nb_samples) {

  // decode the IT data
  std::vector<ALushort> raw_data(nb_samples);
  it_decoder->decode(&raw_data[0], nb_samples);

  // put this decoded data into the buffer
  alBufferData(destination_buffer, AL_FORMAT_STEREO16, &raw_data[0], nb_samples, 44100);

  int error = alGetError();
  Debug::check_assertion(error == AL_NO_ERROR,
      StringConcat() << "Failed to fill the audio buffer with decoded IT data for music file '" << file_name << ": error " << error);
}

/**
 * \brief Decodes a chunk of OGG data into PCM data for the current music.
 * \param destination_buffer the destination buffer to write
 * \param nb_samples number of samples to write
 */
void Music::decode_ogg(ALuint destination_buffer, ALsizei nb_samples) {

  // read the encoded music properties
  vorbis_info* info = ov_info(&ogg_file, -1);
  ALsizei sample_rate = ALsizei(info->rate);

  ALenum al_format = AL_NONE;
  if (info->channels == 1) {
    al_format = AL_FORMAT_MONO16;
  }
  else if (info->channels == 2) {
    al_format = AL_FORMAT_STEREO16;
  }

  // decode the OGG data
  std::vector<ALshort> raw_data(nb_samples * info->channels);
  int bitstream;
  long bytes_read;
  long total_bytes_read = 0;
  long remaining_bytes = nb_samples * info->channels * sizeof(ALshort);
  do {
    bytes_read = ov_read(&ogg_file, ((char*) &raw_data[0]) + total_bytes_read, int(remaining_bytes), 0, 2, 1, &bitstream);
    if (bytes_read < 0) {
      if (bytes_read != OV_HOLE) { // OV_HOLE is normal when the music loops
        Debug::error(StringConcat() << "Error while decoding ogg chunk: "
            << bytes_read);
        return;
      }
    }
    else {
      total_bytes_read += bytes_read;
      remaining_bytes -= bytes_read;
    }
  }
  while (remaining_bytes > 0 && bytes_read > 0);

  // put this decoded data into the buffer
  alBufferData(destination_buffer, al_format, &raw_data[0], ALsizei(total_bytes_read), sample_rate);

  int error = alGetError();
  if (error != AL_NO_ERROR) {
    Debug::error(StringConcat()
        << "Failed to fill the audio buffer with decoded OGG data for music file '"
        << file_name << "': error " << error);
  }
}

/**
 * \brief Loads the file and starts playing this music.
 *
 * No other music should be playing.
 *
 * \return true if the music was loaded successfully
 */
bool Music::start() {

  if (!is_initialized()) {
    return false;
  }

  Debug::check_assertion(current_music == NULL, StringConcat()
      << "Cannot play music '" << id
      << "': a music is already playing");

  // First time: find the file.
  if (file_name.empty()) {
    find_music_file(id, file_name, format);

    if (file_name.empty()) {
      Debug::error(StringConcat() << "Cannot find music file 'musics/" << id
          << "' (tried with extensions .ogg, .it and .spc)");
      return false;
    }
  }

  bool success = true;

  // create the buffers and the source
  alGenBuffers(nb_buffers, buffers);
  alGenSources(1, &source);
  alSourcef(source, AL_GAIN, volume);

  // load the music into memory
  size_t sound_size;
  char* sound_data;
  switch (format) {

    case SPC:

      FileTools::data_file_open_buffer(file_name, &sound_data, &sound_size);

      // load the SPC data into the SPC decoding library
      spc_decoder->load((int16_t*) sound_data, sound_size);
      FileTools::data_file_close_buffer(sound_data);

      for (int i = 0; i < nb_buffers; i++) {
        decode_spc(buffers[i], 4096);
      }
      break;

    case IT:

      FileTools::data_file_open_buffer(file_name, &sound_data, &sound_size);

      // load the IT data into the IT decoding library
      it_decoder->load(sound_data, sound_size);
      FileTools::data_file_close_buffer(sound_data);

      for (int i = 0; i < nb_buffers; i++) {
        decode_it(buffers[i], 4096);
      }
      break;

    case OGG:

    {
      ogg_mem.position = 0;
      ogg_mem.loop = true;
      FileTools::data_file_open_buffer(file_name, &ogg_mem.data, &ogg_mem.size);
      // now, ogg_mem contains the encoded data

      int error = ov_open_callbacks(&ogg_mem, &ogg_file, NULL, 0, Sound::ogg_callbacks);
      if (error) {
        Debug::error(StringConcat() << "Cannot load music file '" << file_name
          << "' from memory: error " << error);
      }
      else {
        for (int i = 0; i < nb_buffers; i++) {
          decode_ogg(buffers[i], 4096);
        }
      }
      break;
    }

    case NO_FORMAT:
      Debug::die("Invalid music format");
      break;
  }

  // start the streaming
  alSourceQueueBuffers(source, nb_buffers, buffers);
  int error = alGetError();
  if (error != AL_NO_ERROR) {
    Debug::error(StringConcat() << "Cannot initialize buffers for music '"
        << file_name << "': error " << error);
    success = false;
  }

  alSourcePlay(source);

  // now the update() function will take care of filling the buffers
  current_music = this;

  return success;
}

/**
 * \brief Stops playing the music.
 */
void Music::stop() {

  if (!is_initialized()) {
    return;
  }

  if (this != current_music) {
    return;
  }

  // empty the source
  alSourceStop(source);

  ALint nb_queued;
  ALuint buffer;
  alGetSourcei(source, AL_BUFFERS_QUEUED, &nb_queued);
  for (int i = 0; i < nb_queued; i++) {
    alSourceUnqueueBuffers(source, 1, &buffer);
  }
  alSourcei(source, AL_BUFFER, 0);

  // delete the source
  alDeleteSources(1, &source);

  // delete the buffers
  alDeleteBuffers(nb_buffers, buffers);

  current_music = NULL;

  switch (format) {

    case SPC:
      break;

    case IT:
      it_decoder->unload();
      break;

    case OGG:
      ov_clear(&ogg_file);
      FileTools::data_file_close_buffer(ogg_mem.data);
      break;

    case NO_FORMAT:
      Debug::die("Invalid music format");
      break;
  }
}

/**
 * \brief Returns whether the music is paused.
 * \return true if the music is paused, false otherwise
 */
bool Music::is_paused() {

  if (!is_initialized()) {
    return false;
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);
  return status == AL_PAUSED;
}

/**
 * \brief Pauses or resumes the music.
 * \param pause true to pause the music, false to resume it
 */
void Music::set_paused(bool pause) {

  if (!is_initialized()) {
    return;
  }

  if (pause) {
    alSourcePause(source);
  }
  else {
    alSourcePlay(source);
  }
}

}

