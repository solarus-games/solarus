/**
 * This modules defines the class Music.
 * The class Music plays a music and
 * also handles the initialization of the whole sound system.
 * The sound and the music are played with the library FMOD 3.75.
 */

#include "Music.h"
#include "FileTools.h"
#include <fmod/fmod_errors.h>

/**
 * Special id indicating that there is no music.
 */
const char *Music::none = "none";

/**
 * Special id indicating that the music is the same as before.
 */
const char *Music::unchanged = "same";

/**
 * True if the sound system has been initialized, i.e. if
 * the method initialize() has been called.
 */
bool Music::initialized = false;

/**
 * Creates a new music.
 * @param music_id id of the music (a file name)
 */
Music::Music(MusicId music_id):
  module(NULL) {

  file_name = (string) FileTools::data_file_get_prefix() + "/music/" + music_id;
}

/**
 * Destroys the music.
 */
Music::~Music(void) {
}

/**
 * Initializes the sound system.
 * This method should be called when the application starts.
 */
void Music::initialize(void) {

  // first we try to initialize FMOD with the default configuration
  if (FSOUND_Init(44100, 32, 0)) {
    initialized = true;
  }
  else {
    // it didn't work: we try with ESD for Linux
    FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
    if (FSOUND_Init(44100, 32, 0)) {
      initialized = true;
    }
    else {
      cerr << "Unable to initialize FMOD: " << FMOD_ErrorString(FSOUND_GetError()) << '\n';
    }
  }
}

/**
 * Closes the sound system.
 * This method should be called when exiting the application.
 */
void Music::exit(void) {
  if (initialized) {
    FSOUND_Close();
    initialized = false;
  }
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
 * Loads the file and plays the music.
 * @return true if the music was loaded successfully, false otherwise
 */
bool Music::play(void) {

  bool success = false;
  if (initialized) {
    module = FMUSIC_LoadSong(file_name.c_str());
    if (module == NULL) {
      cerr << "Unable to play music: " << FMOD_ErrorString(FSOUND_GetError()) << '\n';
    }
    else {
      FMUSIC_SetLooping(module, 0);
      FMUSIC_PlaySong(module);
      success = true;
    }
  }
  return success;
}

/**
 * Stops playing the music.
 */
void Music::stop(void) {
  
  if (initialized) {
    FMUSIC_StopSong(module);
    FMUSIC_FreeSong(module);
  }
}

/**
 * Returns whether the music is paused.
 * @return true if the music is paused, false otherwise
 */
bool Music::is_paused(void) {
  return initialized && FMUSIC_GetPaused(module);
}

/**
 * Pauses or resumes the music.
 * @param pause true to pause the music, false to resume it
 */
void Music::set_paused(bool pause) {
  if (initialized) {
    FMUSIC_SetPaused(module, pause);
  }
}
