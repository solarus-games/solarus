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

bool Sound::initialized = false;

/**
 * Constructor used by the Music subclass.
 */
Sound::Sound(void):
  sample(NULL), chunk(NULL) {

}

/**
 * Creates a new sound.
 * @param sound_id id of the sound (a file name)
 */
Sound::Sound(const SoundId &sound_id):
  sample(NULL), chunk(NULL), channel(0) {

  file_name = (std::string) "sounds/" + sound_id + ".wav";
}

/**
 * Destroys the sound.
 */
Sound::~Sound(void) {
  Sound_FreeSample(sample);
  Mix_FreeChunk(chunk);
}

/**
 * Initializes the music and sound system.
 * This method should be called when the application starts.
 */
void Sound::initialize(void) {

  // SDL_mixer must be initialized with 32 KHz for SPC musics
  int result = Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);

  if (result == -1) {
    std::cerr << "Unable to initialize the sound: " << Mix_GetError()
      << "No music or sound will be played." << std::endl;
    return;
  }

  Mix_AllocateChannels(16);
  Sound_Init();

  Music::initialize();

  initialized = true;
}

/**
 * Closes the music and sound system.
 * This method should be called when exiting the application.
 */
void Sound::quit(void) {
  Music::quit();
  Sound_Quit();
  Mix_CloseAudio();
}

/**
 * Returns whether the music and sound system is initialized.
 * @return true if the audio system is initilialized
 */
bool Sound::is_initialized(void) {
  return initialized;
}

/**
 * Updates the sound system.
 * This function is called repeatedly by the game.
 */
void Sound::update(void) {
  // nothing to do here with SDL_mixer
}

/**
 * Loads the file and plays the sound.
 * @return true if the sound was loaded successfully, false otherwise
 */
bool Sound::play(void) {

  bool success = false;

  if (is_initialized()) {

    if (sample == NULL) {
      SDL_RWops *rw;
      Sound_AudioInfo audio_info = {AUDIO_S16SYS, 2, 32000};
      rw = FileTools::data_file_open_rw(file_name);
      sample = Sound_NewSample(rw, "wav", &audio_info, 1024);

      if (sample == NULL) {
	std::cerr << "Unable to create sound '" << file_name << "': " << Mix_GetError() << std::endl;
      }

      Sound_DecodeAll(sample);

//      FileTools::data_file_close_rw(rw); // TODO cannot free this rw properly
      rw = NULL;
    }

    if (sample != NULL) {
      chunk = Mix_QuickLoad_RAW((Uint8*) sample->buffer, sample->buffer_size);
      channel = Mix_PlayChannel(-1, chunk, 0);

      if (channel == -1) {
	std::cerr << "Unable to play sound '" << file_name << "': " << Mix_GetError() << std::endl;
      }
      else {
	success = true;
      }
    }
  }

  return success;
}

