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

ALCdevice * Sound::device = NULL;
ALCcontext * Sound::context = NULL;
SF_VIRTUAL_IO Sound::sf_virtual;
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
    // create an OpenAL buffer with the sound decoded by the library
    buffer = decode_wav(file_name);

    if (buffer == AL_NONE) {
      std::cerr << "Sound '" << file_name << "' will not be played" << std::endl;
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

  ALCint attr[2] = {ALC_FREQUENCY, 32000}; // 32 KHz is the SPC output sampling rate
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

  // initialize libsndfile
  sf_virtual.get_filelen = sf_get_filelen;
  sf_virtual.seek = sf_seek;
  sf_virtual.read = sf_read;
  sf_virtual.write = sf_write;
  sf_virtual.tell = sf_tell;

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
 * Plays the sound.
 * @return true if the sound was loaded successfully, false otherwise
 */
bool Sound::play(void) {

  bool success = false;

  if (is_initialized() && buffer != AL_NONE) {

    // create a source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);

    // play the sound
    int error = alGetError();
    if (error != AL_NO_ERROR) {
      std::cerr << "Cannot attach buffer " << buffer << " to the source to play sound: error " << error << std::endl;
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

/**
 * Loads the specified wav file and decodes its content into an OpenAL buffer.
 * @param file_name name of the file to open
 * @return the buffer created, or AL_NONE if the sound could not be loaded
 */
ALuint Sound::decode_wav(const std::string &file_name) {

  ALuint buffer = AL_NONE;

  // load the wav file
  WavFromMemory wav;
  wav.position = 0;
  FileTools::data_file_open_buffer(file_name, &wav.data, &wav.size);
  SF_INFO file_info;
  SNDFILE *file = sf_open_virtual(&sf_virtual, SFM_READ, &file_info, &wav);

  if (file == NULL) {
    std::cout << "Cannot load wav file from memory\n";
  }
  else {

    // read the wav properties
    ALsizei nb_samples  = (ALsizei) (file_info.channels * file_info.frames);
    ALsizei sample_rate = (ALsizei) file_info.samplerate;

    // decode the sound with libsndfile
    ALshort *samples = new ALshort[nb_samples];
    if (sf_read_short(file, samples, nb_samples) < nb_samples) {
      std::cout << "Unable to decode wav data\n";
    }
    else {

      ALenum format = AL_NONE;
      if (file_info.channels == 1) {
	format = AL_FORMAT_MONO16;
      }
      else if (file_info.channels == 2) {
	format = AL_FORMAT_STEREO16;
      }

      if (format == AL_NONE) {
	std::cout << "Invalid audio format\n";
      }
      else {

	// copy the samples into an OpenAL buffer
	alGenBuffers(1, &buffer);
        alBufferData(buffer, format, samples, nb_samples * sizeof(ALushort), sample_rate);
	if (alGetError() != AL_NO_ERROR) {
	  std::cout << "Cannot copy the sound sample into buffer " << buffer << "\n";
          buffer = AL_NONE;
        }
      }
    }
    delete[] samples;
    sf_close(file);
  }

  FileTools::data_file_close_buffer(wav.data);

  return buffer;
}


// io functions to load wav from memory

sf_count_t Sound::sf_get_filelen(void *user_data) {

  WavFromMemory *wav = (WavFromMemory*) user_data;
//  std::cout << "get_filelen: " << wav->size << std::endl;
  return wav->size;
}

sf_count_t Sound::sf_seek(sf_count_t offset, int whence, void *user_data) {

  WavFromMemory *wav = (WavFromMemory*) user_data;
//  std::cout << "seek\n";
  
  switch (whence) {

    case SEEK_SET:
      wav->position = offset;
      break;

    case SEEK_CUR:
      wav->position += offset;
      break;

    case SEEK_END:
      wav->position = wav->size - offset;
      break;
  }

  return 0;
}

sf_count_t Sound::sf_read(void *ptr, sf_count_t count, void *user_data) {
  
  WavFromMemory *wav = (WavFromMemory*) user_data;
//  std::cout << "read: " << count << std::endl;
  if (wav->position + count >= wav->size) {
    count = wav->size - wav->position;
  }
  memcpy(ptr, wav->data + wav->position, count);
  wav->position += count;

//  std::cout << count << " bytes read\n";
  return count;
}

sf_count_t Sound::sf_write(const void *ptr, sf_count_t count, void *user_data) {
  // not implemented
  return 0;
}

sf_count_t Sound::sf_tell(void *user_data) {

  WavFromMemory *wav = (WavFromMemory*) user_data;
//  std::cout << "tell: " << wav->position << std::endl;
  return wav->position;
}

