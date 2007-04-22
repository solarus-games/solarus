/**
 * This modules defines the class Music.
 * The class Music plays a music and
 * also handles the initialization of the whole sound system.
 * The sound and the music are played with the library FMOD 3.75.
 */

using namespace std;
#include <iostream>
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <stdlib.h>
#include "music.h"

/**
 * True if the sound system has been initialized, i.e. if
 * the method initialize() has been called.
 */
bool Music::initialized = false;

/**
 * Creates a new music.
 * @param file_name name of the music file (should end with .it)
 */
Music::Music(const char *file_name):
  module(NULL) {
  sprintf(this->file_name, "music/%s", file_name);
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
 * Loads the file and plays the music.
 */
void Music::play(void) {
  if (initialized) {
    module = FMUSIC_LoadSong(file_name);
    if (module == NULL) {
      cerr << "Unable to play music: " << FMOD_ErrorString(FSOUND_GetError()) << '\n';
    }
    else {
      FMUSIC_PlaySong(module);
    }
  }
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
