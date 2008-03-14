#include "Sound.h"
#include "Music.h"
#include "FileTools.h"
#include <fmodex/fmod_errors.h>

/**
 * Creates a new sound.
 * @param sound_id id of the sound (a file name)
 */
Sound::Sound(SoundId sound_id):
  sound(NULL), channel(NULL) {

  file_name = (string) FileTools::data_file_get_prefix() + "/sounds/" + sound_id + ".wav";
}

/**
 * Destroys the sound.
 */
Sound::~Sound(void) {
}

/**
 * Loads the file and plays the sound.
 * @return true if the sound was loaded successfully, false otherwise
 */
bool Sound::play(void) {

  bool success = false;
  FMOD_RESULT result;

  if (Music::is_initialized()) {

    result = FMOD_System_CreateSound(Music::get_fmod_system(), file_name.c_str(), FMOD_LOOP_OFF, 0, &sound);

    if (result != FMOD_OK) {
      cerr << "Unable to create sound '" << file_name << "': " << FMOD_ErrorString(result) << endl;
    }
    else {
      result = FMOD_System_PlaySound(Music::get_fmod_system(), FMOD_CHANNEL_REUSE, sound, false, &channel);

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

  FMOD_BOOL is_playing;
  FMOD_Channel_IsPlaying(channel, &is_playing);
  return is_playing != 0;
}
