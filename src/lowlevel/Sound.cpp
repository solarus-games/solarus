/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include <algorithm>
#include <cstring>  // memcpy
#include <sstream>
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Logger.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/String.h"
#include "solarus/Arguments.h"
#include "solarus/CurrentQuest.h"
#include <cstdio>

namespace Solarus {

ALCdevice* Sound::device = nullptr;
ALCcontext* Sound::context = nullptr;
bool Sound::initialized = false;
bool Sound::sounds_preloaded = false;
float Sound::volume = 1.0;
std::list<Sound*> Sound::current_sounds;
std::map<std::string, Sound> Sound::all_sounds;

namespace {

/**
 * \brief Loads an encoded sound from memory.
 *
 * This function respects the prototype specified by libvorbisfile.
 *
 * \param ptr pointer to a buffer to load
 * \param size size
 * \param nb_bytes number of bytes to load
 * \param datasource source of the data to read
 * \return number of bytes loaded
 */
size_t cb_read(void* ptr, size_t /* size */, size_t nb_bytes, void* datasource) {

  Sound::SoundFromMemory* mem = static_cast<Sound::SoundFromMemory*>(datasource);

  const size_t total_size = mem->data.size();
  if (mem->position >= total_size) {
    if (mem->loop) {
      mem->position = 0;
    }
    else {
      return 0;
    }
  }
  else if (mem->position + nb_bytes >= total_size) {
    nb_bytes = total_size - mem->position;
  }

  std::memcpy(ptr, mem->data.data() + mem->position, nb_bytes);
  mem->position += nb_bytes;

  return nb_bytes;
}

/**
 * \brief Seeks the sound stream to the specified offset.
 *
 * This function respects the prototype specified by libvorbisfile.
 *
 * \param datasource Source of the data to read.
 * \param offset Where to seek.
 * \param whence How to seek: SEEK_SET, SEEK_CUR or SEEK_END.
 * \return 0 in case of success, -1 in case of error.
 */
int cb_seek(void* datasource, ogg_int64_t offset, int whence) {

  Sound::SoundFromMemory* mem = static_cast<Sound::SoundFromMemory*>(datasource);

  switch (whence) {

  case SEEK_SET:
    mem->position = offset;
    break;

  case SEEK_CUR:
    mem->position += offset;
    break;

  case SEEK_END:
    mem->position = mem->data.size() - offset;
    break;
  }

  if (mem->position >= mem->data.size()) {
    mem->position = mem->data.size();
  }

  return 0;
}

/**
 * \brief Returns the current position in a sound stream.
 *
 * This function respects the prototype specified by libvorbisfile.
 *
 * \param datasource Source of the data to read.
 * \return The current position.
 */
long cb_tell(void* datasource) {

  Sound::SoundFromMemory* mem = static_cast<Sound::SoundFromMemory*>(datasource);
  return mem->position;
}

}  // Anonymous namespace.

ov_callbacks Sound::ogg_callbacks = {
    cb_read,
    cb_seek,
    nullptr,  // close
    cb_tell,
};

/**
 * \brief Creates a new Ogg Vorbis sound.
 */
Sound::Sound():
  Sound(std::string("")) {

}

/**
 * \brief Creates a new Ogg Vorbis sound.
 * \param sound_id id of the sound: name of a .ogg file in the sounds subdirectory,
 * without the extension (.ogg is added automatically)
 */
Sound::Sound(const std::string& sound_id):
  id(sound_id),
  buffer(AL_NONE) {

}

/**
 * \brief Destroys the sound.
 */
Sound::~Sound() {

  if (is_initialized() && buffer != AL_NONE) {

    // stop the sources where this buffer is attached
    for (ALuint source: sources) {
      alSourceStop(source);
      alSourcei(source, AL_BUFFER, 0);
      alDeleteSources(1, &source);
    }
    alDeleteBuffers(1, &buffer);
    current_sounds.remove(this);
  }
}

/**
 * \brief Initializes the audio (music and sound) system.
 *
 * This method should be called when the application starts.
 * If the argument -no-audio is provided, this function has no effect and
 * there will be no sound.
 *
 * \param args Command-line arguments.
 */
void Sound::initialize(const Arguments& args) {

  // Check the -no-audio option.
  const bool disable = args.has_argument("-no-audio");
  if (disable) {
    return;
  }

  // Initialize OpenAL.

  device = alcOpenDevice(nullptr);
  if (!device) {
    Debug::error("Cannot open audio device");
    return;
  }

  ALCint attr[] = { ALC_FREQUENCY, 32000, 0 }; // 32 KHz is the SPC output sampling rate
  context = alcCreateContext(device, attr);
  if (!context) {
    Debug::error("Cannot create audio context");
    alcCloseDevice(device);
    return;
  }
  if (!alcMakeContextCurrent(context)) {
    Debug::error("Cannot activate audio context");
    alcDestroyContext(context);
    alcCloseDevice(device);
    return;
  }

  alGenBuffers(0, nullptr);  // Necessary on some systems to avoid errors with the first sound loaded.

  initialized = true;
  set_volume(100);

  // initialize the music system
  Music::initialize();
}

/**
 * \brief Closes the audio (music and sound) system.
 *
 * This method should be called when exiting the application.
 */
void Sound::quit() {

  if (is_initialized()) {

    // uninitialize the music subsystem
    Music::quit();

    // clear the sounds
    all_sounds.clear();

    // uninitialize OpenAL

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    context = nullptr;
    alcCloseDevice(device);
    device = nullptr;
    volume = 1.0;

    initialized = false;
  }
}

/**
 * \brief Returns whether the audio (music and sound) system is initialized.
 * \return true if the audio (music and sound) system is initilialized
 */
bool Sound::is_initialized() {
  return initialized;
}

/**
 * \brief Loads and decodes all sounds listed in the game database.
 */
void Sound::load_all() {

  if (is_initialized() && !sounds_preloaded) {

    const std::map<std::string, std::string>& sound_elements =
        CurrentQuest::get_resources(ResourceType::SOUND);
    for (const auto& kvp: sound_elements) {
      const std::string& sound_id = kvp.first;

      all_sounds[sound_id] = Sound(sound_id);
      all_sounds[sound_id].load();
    }

    sounds_preloaded = true;
  }
}

/**
 * \brief Returns whether a sound exists.
 * \param sound_id id of the sound to test
 * \return true if the sound exists
 */
bool Sound::exists(const std::string& sound_id) {

  std::ostringstream oss;
  oss << "sounds/" << sound_id << ".ogg";
  return QuestFiles::data_file_exists(oss.str());
}

/**
 * \brief Starts playing the specified sound.
 * \param sound_id id of the sound to play
 */
void Sound::play(const std::string& sound_id) {

  if (all_sounds.find(sound_id) == all_sounds.end()) {
    all_sounds[sound_id] = Sound(sound_id);
  }

  all_sounds[sound_id].start();
}

/**
 * \brief Returns the current volume of sound effects.
 * \return the volume (0 to 100)
 */
int Sound::get_volume() {

  return (int) (volume * 100.0 + 0.5);
}

/**
 * \brief Sets the volume of sound effects.
 * \param volume the new volume (0 to 100)
 */
void Sound::set_volume(int volume) {

  volume = std::min(100, std::max(0, volume));
  Sound::volume = volume / 100.0;
  Logger::info(std::string("Sound volume: ") + String::to_string(get_volume()));
}

/**
 * \brief Updates the audio (music and sound) system.
 *
 * This function is called repeatedly by the game.
 */
void Sound::update() {

  // update the playing sounds
  std::list<Sound*> sounds_to_remove;
  for (Sound* sound: current_sounds) {
    if (!sound->update_playing()) {
      sounds_to_remove.push_back(sound);
    }
  }

  for (Sound* sound: sounds_to_remove) {
    current_sounds.remove(sound);
  }

  // also update the music
  Music::update();
}

/**
 * \brief Updates this sound when it is playing.
 * \return true if the sound is still playing, false if it is finished.
 */
bool Sound::update_playing() {

  // See if this sound is still playing.
  if (sources.empty()) {
    return false;
  }

  ALuint source = *sources.begin();
  ALint status;
  alGetSourcei(source, AL_SOURCE_STATE, &status);

  if (status != AL_PLAYING) {
    sources.pop_front();
    alSourcei(source, AL_BUFFER, 0);
    alDeleteSources(1, &source);
  }

  return !sources.empty();
}

/**
 * \brief Loads and decodes the sound into memory.
 */
void Sound::load() {

  if (alGetError() != AL_NONE) {
    Debug::error("Previous audio error not cleaned");
  }

  std::string file_name = std::string("sounds/" + id);
  if (id.find(".") == std::string::npos) {
    file_name += ".ogg";
  }

  // Create an OpenAL buffer with the sound decoded by the library.
  buffer = decode_file(file_name);

  // buffer is now AL_NONE if there was an error.
}

/**
 * \brief Plays the sound.
 * \return true if the sound was loaded successfully, false otherwise
 */
bool Sound::start() {

  bool success = false;

  if (is_initialized()) {

    if (buffer == AL_NONE) { // first time: load and decode the file
      load();
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
        std::ostringstream oss;
        oss << "Cannot attach buffer " << buffer
            << " to the source to play sound '" << id << "': error " << error;
        Debug::error(oss.str());
        alDeleteSources(1, &source);
      }
      else {
        sources.push_back(source);
        current_sounds.remove(this); // to avoid duplicates
        current_sounds.push_back(this);
        alSourcePlay(source);
        error = alGetError();
        if (error != AL_NO_ERROR) {
          std::ostringstream oss;
          oss << "Cannot play sound '" << id << "': error " << error;
          Debug::error(oss.str());
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
 * \brief Loads the specified sound file and decodes its content into an OpenAL buffer.
 * \param file_name name of the file to open
 * \return the buffer created, or AL_NONE if the sound could not be loaded
 */
ALuint Sound::decode_file(const std::string& file_name) {

  ALuint buffer = AL_NONE;

  if (!QuestFiles::data_file_exists(file_name)) {
    Debug::error(std::string("Cannot find sound file '") + file_name + "'");
    return AL_NONE;
  }

  // load the sound file
  SoundFromMemory mem;
  mem.loop = false;
  mem.position = 0;
  mem.data = QuestFiles::data_file_read(file_name);

  OggVorbis_File file;
  int error = ov_open_callbacks(&mem, &file, nullptr, 0, ogg_callbacks);

  if (error) {
    std::ostringstream oss;
    oss << "Cannot load sound file '" << file_name
        << "' from memory: error " << error;
    Debug::error(oss.str());
  }
  else {

    // read the encoded sound properties
    vorbis_info* info = ov_info(&file, -1);
    ALsizei sample_rate = ALsizei(info->rate);

    ALenum format = AL_NONE;
    if (info->channels == 1) {
      format = AL_FORMAT_MONO16;
    }
    else if (info->channels == 2) {
      format = AL_FORMAT_STEREO16;
    }

    if (format == AL_NONE) {
      Debug::error(std::string("Invalid audio format for sound file '")
          + file_name + "'");
    }
    else {
      // decode the sound with vorbisfile
      std::vector<char> samples;
      int bitstream;
      long bytes_read;
      long total_bytes_read = 0;
      const int buffer_size = 16384;
      char samples_buffer[buffer_size];
      do {
        bytes_read = ov_read(&file, samples_buffer, buffer_size, 0, 2, 1, &bitstream);
        if (bytes_read < 0) {
          std::ostringstream oss;
          oss << "Error while decoding ogg chunk in sound file '"
              << file_name << "': " << bytes_read;
          Debug::error(oss.str());
        }
        else {
          total_bytes_read += bytes_read;
          if (format == AL_FORMAT_STEREO16) {
            samples.insert(samples.end(), samples_buffer, samples_buffer + bytes_read);
          }
          else {
            // mono sound files make no sound on some machines
            // workaround: convert them on-the-fly into stereo sounds
            // TODO find a better solution
            for (int i = 0; i < bytes_read; i += 2) {
              samples.insert(samples.end(), samples_buffer + i, samples_buffer + i + 2);
              samples.insert(samples.end(), samples_buffer + i, samples_buffer + i + 2);
            }
            total_bytes_read += bytes_read;
          }
        }
      }
      while (bytes_read > 0);

      // copy the samples into an OpenAL buffer
      alGenBuffers(1, &buffer);
      if (alGetError() != AL_NO_ERROR) {
          Debug::error("Failed to generate audio buffer");
      }
      alBufferData(buffer,
          AL_FORMAT_STEREO16,
          reinterpret_cast<ALshort*>(samples.data()),
          ALsizei(total_bytes_read),
          sample_rate);
      ALenum error = alGetError();
      if (error != AL_NO_ERROR) {
        std::ostringstream oss;
        oss << "Cannot copy the sound samples of '"
            << file_name << "' into buffer " << buffer
            << ": error " << error;
        Debug::error(oss.str());
        buffer = AL_NONE;
      }
    }
    ov_clear(&file);
  }

  mem.data.clear();

  return buffer;
}

}

