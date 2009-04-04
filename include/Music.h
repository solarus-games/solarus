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
#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include "Common.h"
#include "Sound.h"

/**
 * This class represents a music that can be played.
 * A music should be in format .it (Impulse Tracker Module).
 * Before using this class, the audio system should have been
 * initialized, by calling Sound::initialize().
 * Music and Sound should be the only FMOD dependent modules.
 */
class Music: public Sound {

 public:

  static const char *none;
  static const char *unchanged;

  Music(MusicId music_id);
  ~Music(void);

  static bool isNoneId(MusicId music_id);
  static bool isUnchangedId(MusicId music_id);
  static bool isEqualId(MusicId music_id, MusicId other_music_id);

  bool play(void);
  void stop(void);
  bool is_paused(void);
  void set_paused(bool pause);
};

#endif
