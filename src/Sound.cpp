#include "Sound.h"
#include "Music.h"
#include "FileTools.h"
#include <fmodex/fmod_errors.h>

FMOD_SYSTEM *Sound::system = NULL;

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

  file_name = FileTools::data_file_add_prefix("sounds/" + sound_id + ".wav");
}

/**
 * Destroys the sound.
 */
Sound::~Sound(void) {
  if (sound != NULL) {
    FMOD_Sound_Release(sound);
  }
}

/**
 * Initializes the music and sound system.
 * This method should be called when the application starts.
 */
void Sound::initialize(void) {

  FMOD_RESULT result;

  FMOD_System_Create(&system);

  // first we try to initialize FMOD with WinMM instead of DirectSound
  // because we have problems with DirectSound
  FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_WINMM);
  result = FMOD_System_Init(system, 16, FMOD_INIT_NORMAL, NULL);

  if (result != FMOD_OK) {

    // if it doesn't work, we try to auto-detect the output type
    FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_AUTODETECT);
    result = FMOD_System_Init(system, 16, FMOD_INIT_NORMAL, NULL);

    if (result != FMOD_OK) {

      // it didn't work either: try Linux sound
      FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_ALSA);
      result = FMOD_System_Init(system, 16, FMOD_INIT_NORMAL, NULL);

      if (result != FMOD_OK) {
	std::cerr << "Unable to initialize FMOD: " << FMOD_ErrorString(result)
		  << "No music or sound will be played." << std::endl;
	FMOD_System_Release(system);
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
    FMOD_System_Release(system);
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
    FMOD_System_Update(system);
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
      result = FMOD_System_CreateSound(system, file_name.c_str(), FMOD_LOOP_OFF, 0, &sound);

      if (result != FMOD_OK) {
	std::cerr << "Unable to create sound '" << file_name << "': " << FMOD_ErrorString(result) << std::endl;
      }
    }

    if (sound != NULL) {
      result = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sound, false, &channel);

      if (result != FMOD_OK) {
	std::cerr << "Unable to play sound '" << file_name << "': " << FMOD_ErrorString(result) << std::endl;
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

  FMOD_BOOL is_playing;
  FMOD_Channel_IsPlaying(channel, &is_playing);
  return is_playing != 0;
}
