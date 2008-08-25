#include "Sound.h"
#include "Music.h"
#include "FileTools.h"
#include <fmodex/fmod_errors.h>

FMOD::System *Sound::system = NULL;

/**
 * Constructor used by the Music subclass.
 */
Sound::Sound(void) {

}

/**
 * Creates a new sound.
 * @param sound_id id of the sound (a file name)
 */
Sound::Sound(SoundId sound_id):
  sound(NULL), channel(NULL) {

  file_name = FileTools::data_file_add_prefix((string) "/sounds/" + sound_id + ".wav");
}

/**
 * Destroys the sound.
 */
Sound::~Sound(void) {
  if (sound != NULL) {
    sound->release();
  }
}

/**
 * Initializes the music and sound system.
 * This method should be called when the application starts.
 */
void Sound::initialize(void) {

  FMOD_RESULT result;

  FMOD::System_Create(&system);

  // first we try to initialize FMOD with WinMM instead of DirectSound
  // because we have problems with DirectSound
  system->setOutput(FMOD_OUTPUTTYPE_WINMM);
  result = system->init(16, FMOD_INIT_NORMAL, NULL);

  if (result != FMOD_OK) {

    // if it doesn't work, we try to auto-detect the output type
    system->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
    result = system->init(16, FMOD_INIT_NORMAL, NULL);

    if (result != FMOD_OK) {

      // it didn't work either: try Linux sound
      system->setOutput(FMOD_OUTPUTTYPE_ALSA);
      result = system->init(16, FMOD_INIT_NORMAL, NULL);

      if (result != FMOD_OK) {
	cerr << "Unable to initialize FMOD: " << FMOD_ErrorString(result)
	     << "No music or sound will be played." << endl;
	system->release();
	system = NULL;
      }
    }
  }
}

/**
 * Closes the music and sound system.
 * This method should be called when exiting the application.
 */
void Sound::quit(void) {
  if (is_initialized()) {
    system->release();
    system = NULL;
  }
}

/**
 * Returns whether the music and sound system is initialized.
 * @return true if the audio system is initilialized
 */
bool Sound::is_initialized(void) {
  return system != NULL;
}

/**
 * Updates the sound system.
 * This function is called repeatedly by the game.
 */
void Sound::update(void) {
  if (is_initialized()) {
    system->update();
  }
}

/**
 * Loads the file and plays the sound.
 * @return true if the sound was loaded successfully, false otherwise
 */
bool Sound::play(void) {

  bool success = false;
  FMOD_RESULT result;

  if (is_initialized()) {

    if (sound == NULL) {
      result = system->createSound(file_name.c_str(), FMOD_LOOP_OFF, 0, &sound);

      if (result != FMOD_OK) {
	cerr << "Unable to create sound '" << file_name << "': " << FMOD_ErrorString(result) << endl;
      }
    }

    if (sound != NULL) {
      result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);

      if (result != FMOD_OK) {
	cerr << "Unable to play sound '" << file_name << "': " << FMOD_ErrorString(result) << endl;
      }
      else {
	success = true;
      }
    }
  }

  return success;
}

/**
 * Returns whether the sound is playing.
 * @return true if the sound is currently playing
 */
bool Sound::is_playing(void) {

  if (!is_initialized()) {
    return false;
  }

  bool is_playing;
  channel->isPlaying(&is_playing);
  return is_playing;
}
