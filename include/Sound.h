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
#ifndef ZSDX_SOUND_H
#define ZSDX_SOUND_H

#include <fmodex/fmod.h>
#include "Common.h"

/**
 * This class represents a sound effet that can be played in ZSDX.
 * This class also handles the initialization of the whole sound system.
 * The sounds and the musics are played with the library FMOD Ex.
 * To create a sound, prefer the ResourceManager::get_sound() method
 * rather than calling directly the constructor of Sound.
 */
class Sound {

 protected:

  static FMOD_SYSTEM *system;

  FMOD_SOUND *sound;
  FMOD_CHANNEL *channel;

  /**
   * Name of the sound file.
   */
  std::string file_name;

  Sound(void);

 public:

  Sound(SoundId sound_id);
  virtual ~Sound(void);

  static void initialize(void);
  static void quit(void);
  static bool is_initialized(void);
  static void update(void);

  virtual bool play(void);
  bool is_playing(void);
};

#endif
