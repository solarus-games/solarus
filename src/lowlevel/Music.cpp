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
#include "lowlevel/Music.h"
#include "lowlevel/SpcDecoder.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Configuration.h"

SpcDecoder * Music::spc_decoder = NULL;
Music * Music::current_music = NULL;
float Music::volume = 1.0;

const MusicId Music::none = "none";
const MusicId Music::unchanged = "same";

/**
 * @brief Creates a new music.
 * @param music_id id of the music (a file name)
 */
Music::Music(const MusicId &music_id) {

  if (!is_initialized()) {
    return;
  }

  // compute the file name
  file_name = (std::string) "musics/" + music_id;

  // get the format
  size_t index = music_id.find_last_of(".");
  Debug::assert(index != std::string::npos && index != music_id.size(),
    StringConcat() << "Invalid music file name: " << music_id);
  std::string extension = music_id.substr(index + 1);

  if (extension == "spc") {
    format = SPC;
  }
/* not yet implemented
  else if (extension == "it") {
    format = IT;
  }
  */
  else {
    Debug::die(StringConcat() << "Unrecognized music file format: " << music_id);
  }

  for (int i = 0; i < nb_buffers; i++) {
    buffers[i] = AL_NONE;
  }
  source = AL_NONE;
}

/**
 * @brief Destroys the music.
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
 * @brief Initializes the music system.
 */
void Music::initialize() {

  // initialize the SPC decoding feature
  spc_decoder = new SpcDecoder();

  // get the music volume from the configuration file
  set_volume(Configuration::get_value("music_volume", 100));
}

/**
 * @brief Exits the music system.
 */
void Music::quit() {
  if (is_initialized()) {
    delete spc_decoder;
  }
}

/**
 * @brief Returns whether the music system is initialized.
 * @return true if the music system is initilialized
 */
bool Music::is_initialized() {
  return spc_decoder != NULL;
}

/**
 * @brief Returns the current volume of musis.
 * @return the volume (0 to 100)
 */
int Music::get_volume() {

  return (int) (volume * 100.0);
}

/**
 * @brief Sets the volume of musics.
 * @param volume the new volume (0 to 100)
 */
void Music::set_volume(int volume) {

  Debug::assert(volume >= 0 && volume <= 100, StringConcat() << "Illegal volume for music:" << volume);

  Configuration::set_value("music_volume", volume);
  Music::volume = volume / 100.0;

  if (current_music != NULL) {
    alSourcef(current_music->source, AL_GAIN, Music::volume);
  }
}

/**
 * @brief Returns the music currently playing.
 * @return the current music, or NULL if no music is being played
 */
Music* Music::get_current_music() {
  return current_music;
}

/**
 * @brief Updates the music system.
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
 * @brief Updates this music when it is playing.
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
    decode_spc(buffer, 4096);                   // fill it by decoding more SPC data
    alSourceQueueBuffers(source, 1, &buffer);   // queue it again
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);

  if (status != AL_PLAYING) {
    alSourcePlay(source);
  }
}

/**
 * @brief Decodes a chunk of SPC data into PCM data for the current music.
 * @param destination_buffer the destination buffer to write
 * @param nb_samples number of samples to writes
 */
void Music::decode_spc(ALuint destination_buffer, ALsizei nb_samples) {

  // decode the SPC data
  ALushort *raw_data = new ALushort[nb_samples];
  spc_decoder->decode((int16_t*) raw_data, nb_samples);

  // put this decoded data into the buffer
  alBufferData(destination_buffer, AL_FORMAT_STEREO16, raw_data, nb_samples * 2, 32000);

  delete[] raw_data;

  int error = alGetError();
  Debug::assert(error == AL_NO_ERROR,
      StringConcat() << "Failed to fill the audio buffer with decoded SPC data for music file '" << file_name << ": error " << error);
}

/**
 * @brief Loads the file and plays the music.
 *
 * No other music should be playing.
 *
 * @return true if the music was loaded successfully
 */
bool Music::play() {

//  std::cout << "playing music " << file_name << std::endl;

  if (!is_initialized()) {
    return false;
  }

  Debug::assert(format == SPC, StringConcat() << "Cannot play music file '" << file_name << "': unknown music format");
  Debug::assert(current_music == NULL, StringConcat() << "Cannot play music file '" << file_name << "': a music is already playing");

  bool success = true;

  // load the music into memory
  size_t sound_size;
  char *sound_data;
  FileTools::data_file_open_buffer(file_name, &sound_data, &sound_size);

  // load the SPC data into the SPC decoding library
  spc_decoder->load((int16_t*) sound_data, sound_size);
  FileTools::data_file_close_buffer(sound_data);

  // create the two buffers and the source
  alGenBuffers(nb_buffers, buffers);
  alGenSources(1, &source);
  alSourcef(source, AL_GAIN, volume);
  for (int i = 0; i < nb_buffers; i++) {
    decode_spc(buffers[i], 4096);
  }

  // start the streaming
  alSourceQueueBuffers(source, nb_buffers, buffers);
  int error = alGetError();
  if (error != AL_NO_ERROR) {
    std::cerr << "Cannot initialize buffers for music '" << file_name << "': error " << error << std::endl;
    success = false;
  }
/*  else {
    alSourcePlay(source);
    int error = alGetError();
    if (error != AL_NO_ERROR) {
      std::cerr << "Cannot play music '" << file_name << "': error " << error << std::endl;
      success = false;
    }
  }*/

  // now the update() function will take care of filling the buffers
  current_music = this;
  return success;
}

/**
 * @brief Stops playing the music.
 */
void Music::stop() {

//  std::cout << "stopping music " << file_name << std::endl;

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
}

/**
 * @brief Returns whether the music is paused.
 * @return true if the music is paused, false otherwise
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
 * @brief Pauses or resumes the music.
 * @param pause true to pause the music, false to resume it
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

/** TODO
 * @brief Plays a music.
 *
 * If the music is different from the current one,
 * the current one is stopped.
 * The music specified can also be Music::none_id (then the current music is just stopped)
 * or even Music::unchanged_id (nothing is done in this case).
 *
 * @param new_music_id id of the music to play
 */
void Solarus::play_music(const MusicId &new_music_id) {

  if (!Music::is_unchanged_id(new_music_id) && !Music::is_equal_id(new_music_id, current_music_id)) {
    // the music is changed

    previous_music_id = current_music_id; // save the previous music

    if (Music::is_none_id(new_music_id) && current_music != NULL) {

      current_music->stop();
      current_music_id = Music::none;
      current_music = NULL;
    }
    else {

      // play another music
      if (current_music != NULL) {
	current_music->stop();
      }

      Music *new_music = ResourceManager::get_music(new_music_id);

      if (new_music->play()) {
	current_music_id = new_music_id;
	current_music = new_music;
      }
      else {
	current_music_id = Music::none;
	current_music = NULL;
      }
    }
  }
}


