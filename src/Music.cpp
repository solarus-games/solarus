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
 * True if the sound system has been initialized, i.e. if
 * the method initialize() has been called.
 */
bool Music::initialized = false;

FMOD_SYSTEM *Music::system = NULL;
FMOD_CHANNEL *Music::channel = NULL;

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

  FMOD_System_Create(&system);

  // first we try to initialize FMOD with the default configuration
  FMOD_RESULT result = FMOD_System_Init(system, 4, FMOD_INIT_NORMAL, NULL);

  if (result == FMOD_OK) {
    initialized = true;
  }
  else {
    /*
    // it didn't work: we try with ESD for Linux
    FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
    if (FSOUND_Init(44100, 32, 0)) {
      initialized = true;
    }
    else {*/
    cerr << "Unable to initialize FMOD: " << FMOD_ErrorString(result) << '\n';
    //   }
  }
}

/**
 * Closes the sound system.
 * This method should be called when exiting the application.
 */
void Music::exit(void) {
  if (initialized) {
    FMOD_System_Release(system);
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
  FMOD_RESULT result;

  if (initialized) {

    result = FMOD_System_CreateSound(system, file_name.c_str(), FMOD_LOOP_OFF, 0, &module);

    if (result != FMOD_OK) {
      cerr << "Unable to create music '" << file_name << "': " << FMOD_ErrorString(result) << endl;
    }
    else {
      result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, module, false, &channel);

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
  
  if (initialized) {
  
    FMOD_RESULT result = FMOD_Channel_Stop(channel);

    if (result != FMOD_OK) {
      cerr << "Cannot stop the module: " << FMOD_ErrorString(result) << endl;
    }
    FMOD_Sound_Release(module);
  }
}

/**
 * Returns whether the music is paused.
 * @return true if the music is paused, false otherwise
 */
bool Music::is_paused(void) {

  if (!initialized) {
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
  if (initialized) {
    FMOD_Channel_SetPaused(channel, pause);
  }
}
