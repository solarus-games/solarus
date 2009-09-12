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
#include <AL/alut.h>

ALCdevice * Sound::device = NULL;
ALCcontext * Sound::context = NULL;
bool Sound::initialized = false;
std::list<Sound*> Sound::current_sounds;

/**
 * Creates a new sound.
 * @param sound_id id of the sound (a file name)
 */
Sound::Sound(const SoundId &sound_id):
  buffer(AL_NONE) {

  if (is_initialized()) {

    std::string file_name = (std::string) "sounds/" + sound_id + ".wav";

    // load the wav file
    size_t wav_size;
    char *wav_data;
    FileTools::data_file_open_buffer(file_name, &wav_data, &wav_size);

    // create an OpenAL buffer with the sound decoded by ALUT
    buffer = alutCreateBufferFromFileImage((ALvoid*) wav_data, wav_size);
    FileTools::data_file_close_buffer(wav_data);

    if (buffer == AL_NONE) {
      std::cerr << "Cannot decode WAV data for sound '" << file_name << "': " << alutGetErrorString(alutGetError()) << std::endl;
    }
  }
}

/**
 * Destroys the sound.
 */
Sound::~Sound(void) {

  if (is_initialized()) {

    // stop the sources where this buffer is attached
    std::list<ALuint>::iterator it;
    for (it = sources.begin(); it != sources.end(); it++) {
      ALuint source = (*it);
      alSourceStop(source);
      alSourcei(source, AL_BUFFER, 0);
      alDeleteSources(1, &source);
    }
    alDeleteBuffers(1, &buffer);
    current_sounds.remove(this);
  }
}

/**
 * Initializes the audio (music and sound) system.
 * This method should be called when the application starts.
 */
void Sound::initialize(void) {

  // initialize OpenAL
  /*
  if (!alutInit(NULL, NULL)) {
    std::cout << "Cannot initialize alut: " << alutGetErrorString(alutGetError()) << std::endl;
    return;
  }
  */

  alutInitWithoutContext(NULL, NULL);
/*
  const ALCchar* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
  while (devices[0] != '\0') {
    std::cout << "Audio device: " << devices << std::endl;
    devices += strlen(devices) + 1;
  }
  */

  device = alcOpenDevice(NULL);
//  device = alcOpenDevice("ALSA Software on ATI IXP");
  if (!device) {
    std::cout << "Cannot open audio device" << std::endl;
    return;
  }

  const int attr[2] = {ALC_FREQUENCY, 32000}; // 32 KHz is the SPC output sampling rate
  context = alcCreateContext(device, attr);
  if (!context) {
    std::cout << "Cannot create audio context" << std::endl;
    alcCloseDevice(device);
    return;
  }
  if (!alcMakeContextCurrent(context)) {
    std::cout << "Cannot activate audio context" << std::endl;
    alcDestroyContext(context);
    alcCloseDevice(device);
    return;
  }

  initialized = true;

  // initialize the music system
  Music::initialize();
}

/**
 * Closes the audio (music and sound) system.
 * This method should be called when exiting the application.
 */
void Sound::quit(void) {

  if (is_initialized()) {
/*
    // stop the sound sources
    ALuint source;
    std::list<ALuint>::iterator it;
    for (it = all_sources.begin(); it != all_sources.end(); it++) {
      source = (*it);
      alSourcei(source, AL_BUFFER, 0);
      alDeleteSources(1, &source);
    }
*/
    // uninitialize the music subsystem
    Music::quit();

    // uninitialize OpenAL

    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    context = NULL;
    alcCloseDevice(device);
    device = NULL;

    alutExit();
    initialized = false;
  }
}

/**
 * Returns whether the audio (music and sound) system is initialized.
 * @return true if the audio (music and sound) system is initilialized
 */
bool Sound::is_initialized(void) {
  return initialized;
}

/**
 * Updates the audio (music and sound) system.
 * This function is called repeatedly by the game.
 */
void Sound::update(void) {

  // update the playing sounds
  Sound *sound;
  std::list<Sound*> sounds_to_remove;
  std::list<Sound*>::iterator it;
  for (it = current_sounds.begin(); it != current_sounds.end(); it++) {
    sound = *it;
    if (!sound->update_playing()) {
      sounds_to_remove.push_back(sound);
    }
  }

  for (it = sounds_to_remove.begin(); it != sounds_to_remove.end(); it++) {
    sound = *it;
    current_sounds.remove(sound);
  }

  // also update the music
  Music::update();
}

/**
 * Updates this sound when it is playing.
 * @return true if the sound is still playing, false if it is finished.
 */
bool Sound::update_playing() {

  // see whether a source playing this sound has finished playing
  ALuint source = *sources.begin();
  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);

  if (status != AL_PLAYING) {
    sources.pop_front();
    alSourcei(source, AL_BUFFER, 0);
    alDeleteSources(1, &source);
  }

  return sources.size() != 0;
}

/**
 * Loads the file and plays the sound.
 * @return true if the sound was loaded successfully, false otherwise
 */
bool Sound::play(void) {

  bool success = false;

  if (is_initialized()) {

    // create a source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

    // play the sound
    int error = alGetError();
    if (error != AL_NO_ERROR) {
      std::cerr << "Cannot attach the buffer to the source to play sound: error " << error << std::endl;
      alDeleteSources(1, &source);
    }
    else {
      sources.push_back(source);
      current_sounds.remove(this); // to avoid duplicates
      current_sounds.push_back(this);
      alSourcePlay(source);
      error = alGetError();
      if (error != AL_NO_ERROR) {
	std::cerr << "Cannot play sound: error " << error << std::endl;
      }
      else {
	success = true;
      }
    }
  }
  return success;
}

