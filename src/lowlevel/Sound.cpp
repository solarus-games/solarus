/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream> // std::cerr
#include <cstring>  // memcpy
#include <cmath>
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Configuration.h"

ALCdevice * Sound::device = NULL;
ALCcontext * Sound::context = NULL;
SF_VIRTUAL_IO Sound::sf_virtual;
bool Sound::initialized = false;
float Sound::volume = 1.0;
std::list<Sound*> Sound::current_sounds;
std::map<SoundId,Sound> Sound::all_sounds;

/**
 * @brief Empty constructor.
 */
Sound::Sound():
  id(""), buffer(AL_NONE) {

}

/**
 * @brief Creates a new Ogg Vorbis sound.
 * @param sound_id id of the sound: name of a .ogg file in the sounds subdirectory,
 * without the extension (.ogg is added automatically)
 */
Sound::Sound(const SoundId &sound_id):
  id(sound_id), buffer(AL_NONE) {
}

/**
 * @brief Destroys the sound.
 */
Sound::~Sound() {

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
 * @brief Initializes the audio (music and sound) system.
 *
 * This method should be called when the application starts.
 * If the argument -no-audio is provided, this function has no effect and
 * there will be no sound.
 *
 * @param argc command-line arguments number
 * @param argv command-line arguments
 */
void Sound::initialize(int argc, char **argv) {
 
  // check the -no-audio option
  bool disable = false;
  for (argv++; argc > 1 && !disable; argv++, argc--) {
    const std::string arg = *argv;
    disable = (arg.find("-no-audio") == 0);
  }
  if (disable) {
    return;
  }

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

  ALCint attr[] = { ALC_FREQUENCY, 32000, 0 }; // 32 KHz is the SPC output sampling rate
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

  // get the sound effects volume from the configuration file
  set_volume(Configuration::get_value("sound_volume", 100));

  // initialize the music system
  Music::initialize();
}

/**
 * @brief Closes the audio (music and sound) system.
 *
 * This method should be called when exiting the application.
 */
void Sound::quit() {

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

    // clear the sounds
    all_sounds.clear();

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
 * @brief Returns whether the audio (music and sound) system is initialized.
 * @return true if the audio (music and sound) system is initilialized
 */
bool Sound::is_initialized() {
  return initialized;
}

/**
 * @brief Starts playing the specified sound.
 * @param sound_id id of the sound to play
 */
void Sound::play(const SoundId &sound_id) {

  if (all_sounds.count(sound_id) == 0) {
    all_sounds[sound_id] = Sound(sound_id);
  }

  all_sounds[sound_id].start();
}

/**
 * @brief Returns the current volume of sound effects.
 * @return the volume (0 to 100)
 */
int Sound::get_volume() {

  return (int) (volume * 100.0);
}

/**
 * @brief Sets the volume of sound effects.
 * @param volume the new volume (0 to 100)
 */
void Sound::set_volume(int volume) {

  Debug::assert(volume >= 0 && volume <= 100, StringConcat() << "Illegal volume for sound effects:" << volume);

  Configuration::set_value("sound_volume", volume);
  Sound::volume = volume / 100.0;
}

/**
 * @brief Updates the audio (music and sound) system.
 *
 * This function is called repeatedly by the game.
 */
void Sound::update() {

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
 * @brief Updates this sound when it is playing.
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
 * @brief Plays the sound.
 * @return true if the sound was loaded successfully, false otherwise
 */
bool Sound::start() {

  bool success = false;

  if (is_initialized()) {

    if (buffer == AL_NONE) { // first time: load and decode the file

      std::string file_name = (std::string) "sounds/" + id;
      if (id.find(".") == std::string::npos) {
	file_name += ".ogg";
      }

      // create an OpenAL buffer with the sound decoded by the library
      buffer = decode_file(file_name);

      if (buffer == AL_NONE) {
	std::cerr << "Sound '" << file_name << "' will not be played" << std::endl;
      }
    }

    if (buffer != AL_NONE) {

      // create a source
      ALuint source;
      alGenSources(1, &source);
      alSourcei(source, AL_BUFFER, buffer);
      alSourcef(source, AL_GAIN, volume);

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
  }
  return success;
}

/**
 * @brief Loads the specified sound file and decodes its content into an OpenAL buffer.
 * @param file_name name of the file to open
 * @return the buffer created, or AL_NONE if the sound could not be loaded
 */
ALuint Sound::decode_file(const std::string &file_name) {

  ALuint buffer = AL_NONE;

  // load the sound file
  SoundFromMemory mem;
  mem.position = 0;
  FileTools::data_file_open_buffer(file_name, &mem.data, &mem.size);
  SF_INFO file_info = {0, 0, 0, 0, 0, 0};
  SNDFILE *file = sf_open_virtual(&sf_virtual, SFM_READ, &file_info, &mem);

  if (file == NULL) {
    std::cout << "Cannot load sound file from memory\n";
  }
  else {

    // read the encoded sound properties
    ALsizei nb_samples  = (ALsizei) (file_info.channels * file_info.frames);
    ALsizei sample_rate = (ALsizei) file_info.samplerate;

    // decode the sound with libsndfile
    ALshort *samples = new ALshort[nb_samples];
    if (sf_read_short(file, samples, nb_samples) < nb_samples) {
      std::cout << "Unable to decode sound data\n";
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

  FileTools::data_file_close_buffer(mem.data);

  return buffer;
}


// io functions to load the encoded sound from memory

sf_count_t Sound::sf_get_filelen(void *user_data) {

  SoundFromMemory *mem = (SoundFromMemory*) user_data;
  return mem->size;
}

sf_count_t Sound::sf_seek(sf_count_t offset, int whence, void *user_data) {

  SoundFromMemory *mem = (SoundFromMemory*) user_data;

  switch (whence) {

    case SEEK_SET:
      mem->position = offset;
      break;

    case SEEK_CUR:
      mem->position += offset;
      break;

    case SEEK_END:
      mem->position = mem->size - offset;
      break;
  }

  return 0;
}

sf_count_t Sound::sf_read(void *ptr, sf_count_t count, void *user_data) {

  SoundFromMemory *mem = (SoundFromMemory*) user_data;
  if (mem->position + count >= mem->size) {
    count = mem->size - mem->position;
  }
  memcpy(ptr, mem->data + mem->position, count);
  mem->position += count;

  return count;
}

sf_count_t Sound::sf_write(const void *ptr, sf_count_t count, void *user_data) {
  // not implemented
  return 0;
}

sf_count_t Sound::sf_tell(void *user_data) {

  SoundFromMemory *mem = (SoundFromMemory*) user_data;
  return mem->position;
}

