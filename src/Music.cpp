/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 *
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Music.h"
#include "FileTools.h"

SNES_SPC * Music::spc_manager = NULL;
SPC_Filter * Music::spc_filter = NULL;

Music * Music::current_music = NULL;

const MusicId Music::none = "none";
const MusicId Music::unchanged = "same";


/**
 * Creates a new music.
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
  if (index == std::string::npos || index == music_id.size()) {
    DIE("Invalid music file name: " << music_id);
  }
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
    DIE("Unrecognized music file format: " << music_id);
  }

  buffers[0] = AL_NONE;
  buffers[1] = AL_NONE;
  source = AL_NONE;
}

/**
 * Destroys the music.
 */
Music::~Music(void) {

  if (!is_initialized()) {
    return;
  }

  if (current_music == this) {
    stop();
  }
}

/**
 * Initializes the music system.
 */
void Music::initialize(void) {

  // initialize the SPC library
  spc_manager = spc_new();
  spc_filter = spc_filter_new();
}

/**
 * Exits the music system.
 */
void Music::quit(void) {
  if (is_initialized()) {
    spc_filter_delete(spc_filter);
    spc_delete(spc_manager);
  }
}

/**
 * Returns whether the music system is initialized.
 * @return true if the music system is initilialized
 */
bool Music::is_initialized(void) {
  return spc_manager != NULL;
}

/**
 * Updates the music system.
 * When a music is playing, this function makes it update.
 */
void Music::update(void) {
  
  if (!is_initialized()) {
    return;
  }

  if (current_music != NULL) {
    current_music->update_playing();
  }
}

/**
 * Updates this music when it is playing.
 * This function handles the double buffering.
 */
void Music::update_playing(void) {
  ALint status, nb_empty;

  // get the empty buffers
  alGetSourcei(source, AL_BUFFERS_PROCESSED, &nb_empty);

  // refill them (note that when everything is fine, only one may be empty) 
  for (int i = 0; i < nb_empty; i++) {
    ALuint buffer;
    alSourceUnqueueBuffers(source, 1, &buffer); // unqueue the buffer
    decode_spc(buffer, 32768);                  // fill it by decoding more SPC data
    alSourceQueueBuffers(source, 1, &buffer);   // queue it again
  }

  // see if the music is finished
  alGetSourcei(source, AL_SOURCE_STATE, &status);

  if (status != AL_PLAYING) {
    stop(); // the end was reached or there was an error
  }
}

/**
 * Decodes a chunk of SPC data into PCM data for the current music.
 * @param destination_buffer the destination buffer to write
 * @param nb_samples number of samples to writes
 */
void Music::decode_spc(ALuint destination_buffer, ALsizei nb_samples) {

  // decode the SPC data
  ALushort *raw_data = new ALushort[nb_samples];
  const char *err = spc_play(spc_manager, nb_samples, (short int*) raw_data);
  if (err != NULL) {
    DIE("Failed to decode SPC data for music file '" << file_name << ": " << err);
  }
  spc_filter_run(spc_filter, (short int*) raw_data, nb_samples);

  // put this decoded data into the buffer
  alBufferData(destination_buffer, AL_FORMAT_STEREO16, raw_data, nb_samples * 2, 32000);

  delete[] raw_data;

  int error = alGetError();
  if (error != AL_NO_ERROR) {
    DIE("Failed to fill the audio buffer with decoded SPC data for music file '" << file_name << ": error " << error);
  }
}

/**
 * Loads the file and plays the music.
 * @return true if the music was loaded successfully
 */
bool Music::play(void) {

  if (!is_initialized()) {
    return false;
  }

  if (format != SPC) {
    DIE("Cannot play music file '" << file_name << "': unknown music format");
  }

  if (current_music != NULL) {
    DIE("Cannot play music file '" << file_name << "': a music is already playing");
  }

  bool success = true;

  // load the music into memory
  size_t sound_size;
  char *sound_data;
  FileTools::data_file_open_buffer(file_name, &sound_data, &sound_size);

  // load the SPC data into the SPC decoding library
  spc_load_spc(spc_manager, (short int*) sound_data, sound_size);
  spc_clear_echo(spc_manager);
  spc_filter_clear(spc_filter);
  FileTools::data_file_close_buffer(sound_data);

  // create the two buffers and the source
  alGenBuffers(2, buffers);
  alGenSources(1, &source);

  decode_spc(buffers[0], 32768);
  decode_spc(buffers[1], 8192);

  // start the streaming
  alSourceQueueBuffers(source, 2, buffers);;
  int error = alGetError();
  if (error != AL_NO_ERROR) {
    std::cerr << "Cannot initialize buffers for music '" << file_name << "': error " << error << std::endl;
    success = false;
  }
  else {
    alSourcePlay(source);
    int error = alGetError();
    if (error != AL_NO_ERROR) {
      std::cerr << "Cannot play music '" << file_name << "': error " << error << std::endl;
      success = false;
    }
  }
  
  // now the update() function will take care of filling the buffers
  current_music = this;
  return success;
}

/**
 * Stops playing the music.
 */
void Music::stop(void) {

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
  alDeleteBuffers(2, buffers);

  current_music = NULL;
}

/**
 * Returns whether the music is paused.
 * @return true if the music is paused, false otherwise
 */
bool Music::is_paused(void) {

  if (!is_initialized()) {
    return false;
  }

  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);
  return status == AL_PAUSED;
}

/**
 * Pauses or resumes the music.
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

/**
 * Returns whether a music id is the id for no music, i.e. if it is Music.none_id.
 * @param music_id a music id
 * @return true if music_id is the special id indicating that there is no music
 */
bool Music::isNoneId(const MusicId &music_id) {
  return isEqualId(music_id, none);
}

/**
 * Returns whether a music id is the id for no change, i.e. if it is Music.unchanged_id.
 * @param music_id a music id
 * @return true if music_id is the special id indicating that the music doesn't change
 */
bool Music::isUnchangedId(const MusicId &music_id) {
  return isEqualId(music_id, unchanged);
}

/**
 * Compares two music ids.
 * @param music_id a music id
 * @param other_music_id another music id
 * @return true if the ids are the same
 */
bool Music::isEqualId(const MusicId &music_id, const MusicId &other_music_id) {
  return music_id == other_music_id;
}

