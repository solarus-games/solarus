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
  file_name = (string) FileTools::data_file_get_prefix() + "/music/" + music_id;

  /*
   * The musics are played with the highest priority.
   * Otherwise, they would be interrupted by the sound effects sometimes.
   */
  FMOD_System_GetChannel(system, 0, &channel);
  FMOD_Channel_SetPriority(channel, 0);
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

  //  cout << "testing if two music ids are equal: '" << music_id << "' and '" << other_music_id << "': " << (music_id == other_music_id) << endl;
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

    result = FMOD_System_CreateStream(system, file_name.c_str(), FMOD_LOOP_NORMAL, 0, &sound);

    if (result != FMOD_OK) {
      cerr << "Unable to create music '" << file_name << "': " << FMOD_ErrorString(result) << endl;
    }
    else {
      
      result = FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, sound, false, &channel);

      if (result != FMOD_OK) {
	cerr << "Unable to play music '" << file_name << "': " << FMOD_ErrorString(result) << endl;
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
      cerr << "The music '" << file_name << "' is already stopped.\n";
    }
    else {
  
      FMOD_RESULT result = FMOD_Channel_Stop(channel);
      
      if (result != FMOD_OK) {
	cerr << "Cannot stop the music: " << FMOD_ErrorString(result) << endl;
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

  return pause;
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
