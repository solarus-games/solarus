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
#include <fmodex/fmod_errors.h>

SNES_SPC *Music::snes_spc;
SPC_Filter *Music::filter;

/**
 * Special id indicating that there is no music.
 */
const MusicId Music::none = "none";

/**
 * Special id indicating that the music is the same as before.
 */
const MusicId Music::unchanged = "same";

/**
 * Creates a new music.
 * @param music_id id of the music (a file name)
 */
Music::Music(const MusicId &music_id) {

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
  else if (extension == "it") {
    format = IT;
  }
  else {
    DIE("Unrecognized music file format: " << music_id);
  }

  // play musics with the highest priority to avoid being interrupted by sound effects
  FMOD_System_GetChannel(system, 15, &channel);
  FMOD_Channel_SetPriority(channel, 15);
  //  FMOD_Channel_SetFrequency(channel, 32000);
}

/**
 * Destroys the music.
 */
Music::~Music(void) {

}

/**
 * Initializes the music system.
 */
void Music::initialize(void) {

  if (is_initialized()) {
    // initialize the SPC library
    snes_spc = spc_new();
    filter = spc_filter_new();
  }
}

/**
 * Exits the music system.
 */
void Music::quit(void) {
  if (is_initialized()) {
    spc_filter_delete(filter);
    spc_delete(snes_spc);
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

/**
 * Loads the file and plays the music.
 * @return true if the music was loaded successfully
 */
bool Music::play(void) {

  bool success = false;

  if (is_initialized()) {

    // load the music into memory
    size_t sound_size;
    char *sound_data;
    FileTools::data_file_open_buffer(file_name, &sound_data, &sound_size);

    // tell FMOD the music properties
    FMOD_CREATESOUNDEXINFO ex = {0};
    ex.cbsize = sizeof(ex);
    ex.length = sound_size;

    FMOD_RESULT result;
    if (format != SPC) {
      // the music has a format recognized by FMOD (e.g. Impulse Tracker)
      result = FMOD_System_CreateStream(system, sound_data, FMOD_LOOP_NORMAL | FMOD_OPENMEMORY, &ex, &sound);
    }
    else {
      // SPC format: we need to give FMOD the format properties and the decoding function

      ex.numchannels = 2;                  // stereo
      ex.defaultfrequency = 32000;         // 32 KHz (note: IT files converted with OpenSPC Lite seem to have a wrong sample rate of 32768)
      ex.format = FMOD_SOUND_FORMAT_PCM16; // PCM 16 bits
      ex.pcmreadcallback = spc_callback;
      ex.decodebuffersize = 4096;          // problems without this line

      spc_load_spc(snes_spc, (short int*) sound_data, sound_size);
      spc_clear_echo(snes_spc);
      spc_filter_clear(filter);

      result = FMOD_System_CreateStream(system, sound_data, FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_OPENMEMORY, &ex, &sound);
    }

    if (result != FMOD_OK) {
      std::cerr << "Unable to create music '" << file_name << "': " << FMOD_ErrorString(result) << std::endl;
    }
    else {
      result = FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, sound, false, &channel);

      if (result != FMOD_OK) {
	std::cerr << "Unable to play music '" << file_name << "': " << FMOD_ErrorString(result) << std::endl;
      }
      else {
	success = true;
      }
    }
    FileTools::data_file_close_buffer(sound_data);
  }

  return success;
}

/**
 * Custom music function called by the sound library to play SPC files.
 * @param sound the sound to decode
 * @param data buffer to write with the 32 KHz 16-bit stereo raw sound decoded by the SPC library
 * @param datalen number of bytes to write into the buffer
 */
FMOD_RESULT F_CALLBACK Music::spc_callback(FMOD_SOUND *sound, void *data, unsigned int datalen) {

  int words = datalen / 2; // the SPC library wants the number of 16-bit words
  spc_play(snes_spc, words, (short int*) data);
  spc_filter_run(filter, (short int*) data, words);

  return FMOD_OK;
}

/**
 * Stops playing the music.
 */
void Music::stop(void) {

  if (is_initialized()) {

    FMOD_RESULT result = FMOD_Channel_Stop(channel);

    if (result != FMOD_OK) {
      std::cerr << "Cannot stop the music: " << FMOD_ErrorString(result) << std::endl;
    }

    FMOD_Sound_Release(sound);
    sound = NULL;
  }
}

/**
 * Returns whether the music is paused.
 * @return true if the music is paused, false otherwise
 */
bool Music::is_paused(void) {

  if (!is_initialized()) {
    return false;
  }

  FMOD_BOOL pause;
  FMOD_Channel_GetPaused(channel, &pause);

  return pause != 0;
}

/**
 * Pauses or resumes the music.
 * @param pause true to pause the music, false to resume it
 */
void Music::set_paused(bool pause) {
  if (is_initialized()) {
    FMOD_Channel_SetPaused(channel, pause);
  }
}

