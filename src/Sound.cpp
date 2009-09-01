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
#include "Sound.h"
#include "Music.h"
#include "FileTools.h"
#include <fmodex/fmod_errors.h>

FMOD_SYSTEM *Sound::system = NULL;

/**
 * Constructor used by the Music subclass.
 */
Sound::Sound(void):
  sound(NULL), channel(NULL) {

}

/**
 * Creates a new sound.
 * @param sound_id id of the sound (a file name)
 */
Sound::Sound(const SoundId &sound_id):
  sound(NULL), channel(NULL) {

  file_name = (std::string) "sounds/" + sound_id + ".wav";
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

  /* not working
  // initialize the default sample rate we want
  int sample_rate, num_output_channels, max_input_channels, bits;
  FMOD_SOUND_FORMAT sound_format;
  FMOD_DSP_RESAMPLER resampler_method;
  FMOD_System_GetSoftwareFormat(system, &sample_rate, &sound_format, &num_output_channels, &max_input_channels, &resampler_method, &bits);
  result = FMOD_System_SetSoftwareFormat(system, 32000, sound_format, num_output_channels, max_input_channels, resampler_method);
  if (result != FMOD_OK) {
    std::cout << "Warning: failed to set the default sample rate to 32 KHz: " << FMOD_ErrorString(result) << std::endl;
  }

  //  FMOD_System_GetSoftwareFormat(system, &sample_rate, &sound_format, &num_output_channels, &max_input_channels, &resampler_method, &bits);
  //  std::cout << "sample rate: " << sample_rate << ", sound format: " << sound_format << ", num output channels: " << num_output_channels
  //  << ", max input channels: " << max_input_channels << ", resampler method: " << resampler_method << ", bits: " << bits << std::endl;
  */

  // first we try to initialize FMOD with WinMM instead of DirectSound
  // because we have problems with DirectSound
  FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_WINMM);
  result = FMOD_System_Init(system, 16, FMOD_INIT_NORMAL, NULL);

  if (result != FMOD_OK) {

    // if it didn't work, we try to auto-detect the output type
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

  if (result == FMOD_OK) {
    Music::initialize();
  }
}

/**
 * Closes the music and sound system.
 * This method should be called when exiting the application.
 */
void Sound::quit(void) {
  Music::quit();
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

      size_t size;
      char *buffer;
      FileTools::data_file_open_buffer(file_name, &buffer, &size);
      FMOD_CREATESOUNDEXINFO ex = {0};
      ex.cbsize = sizeof(ex);
      ex.length = size;
      result = FMOD_System_CreateSound(system, buffer, FMOD_LOOP_OFF | FMOD_OPENMEMORY, &ex, &sound);
      FileTools::data_file_close_buffer(buffer);

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

