/**
 * This module defines the class Sound.
 */

#ifndef ZSDX_SOUND_H
#define ZSDX_SOUND_H

#include <fmodex/fmod.h>
#include "Common.h"

/**
 * This class represents a sound effet that can be played in ZSDX.
 * Before using this class, the audio system should have been
 * initialized, by calling Music::initialize().
 */
class Sound {

 private:
  FMOD_SOUND *sound;
  FMOD_CHANNEL *channel;

 public:

  /**
   * Name of the sound file.
   */
  string file_name;

  Sound(SoundId sound_id);
  ~Sound(void);

  bool play(void);
  bool is_playing(void);
};

#endif
