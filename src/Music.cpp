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

/**
 * Special id indicating that there is no music.
 */
const char *Music::none = "none";

/**
 * Special id indicating that the music is the same as before.
 */
const char *Music::unchanged = "same";

/**
 * Creates a new music.
 * @param music_id id of the music (a file name)
 */
Music::Music(MusicId music_id) {

  sound = NULL;
  file_name = FileTools::data_file_add_prefix("musics/" + music_id);

  /*
   * The musics are played with the highest priority.
   * Otherwise, they would be sometimes interrupted by the sound effects.
   */
  if (is_initialized()) {
    FMOD_System_GetChannel(system, 15, &channel);
    FMOD_Channel_SetPriority(channel, 15);
  }
}

/**
 * Destroys the music.
 */
Music::~Music(void) {
}

/**
 * Returns whether a music id is the id for no music, i.e. if it is Music.none_id.
 * @param music_id a music id
 * @return true if music_id is the special id indicating that there is no music
 */
bool Music::isNoneId(MusicId music_id) {
  return isEqualId(music_id, none);
}

/**
 * Returns whether a music id is the id for no change, i.e. if it is Music.unchanged_id.
 * @param music_id a music id
 * @return true if music_id is the special id indicating that the music doesn't change
 */
bool Music::isUnchangedId(MusicId music_id) {
  return isEqualId(music_id, unchanged);
}

/**
 * Compares two music ids.
 * @param music_id a music id
 * @param other_music_id another music id
 * @return true if the ids are the same
 */
bool Music::isEqualId(MusicId music_id, MusicId other_music_id) {
  return music_id == other_music_id;
}

/**
 * Loads the file and plays the music repeatedly.
 * @return true if the music was loaded successfully, false otherwise
 */
bool Music::play(void) {

  bool success = false;
  FMOD_RESULT result;

  if (is_initialized()) {

/*  The beginning of any .it music is badly played on my windows with DirectX
    other workarounds: use FMOD_SOFTWARE or FMOD_OUTPUTTYPE_WINMM?
    */
    result = FMOD_System_CreateStream(system, file_name.c_str(), FMOD_LOOP_NORMAL, 0, &sound);

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
  }

  return success;
}

/**
 * Stops playing the music.
 */
void Music::stop(void) {

  if (is_initialized()) {

    if (!is_playing()) {
      std::cerr << "The music '" << file_name << "' is already stopped." << std::endl;
    }
    else {

      FMOD_RESULT result = FMOD_Channel_Stop(channel);

      if (result != FMOD_OK) {
	std::cerr << "Cannot stop the music: " << FMOD_ErrorString(result) << std::endl;
      }
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
