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
#include "Music.h"
#include "FileTools.h"

/**
 * Special id indicating that there is no music.
 */
const MusicId Music::none = "none";

/**
 * Special id indicating that the music is the same as before.
 */
const MusicId Music::unchanged = "same";

/**
 * Creates a new music.
 * @param music_id id of the music (a file name)
 */
Music::Music(const MusicId &music_id):
  music(NULL) {

  file_name = (std::string) "musics/" + music_id;
}

/**
 * Destroys the music.
 */
Music::~Music(void) {
  Mix_FreeMusic(music);
}

/**
 * Returns whether a music id is the id for no music, i.e. if it is Music.none_id.
 * @param music_id a music id
 * @return true if music_id is the special id indicating that there is no music
 */
bool Music::isNoneId(const MusicId &music_id) {
  return isEqualId(music_id, none);
}

/**
 * Returns whether a music id is the id for no change, i.e. if it is Music.unchanged_id.
 * @param music_id a music id
 * @return true if music_id is the special id indicating that the music doesn't change
 */
bool Music::isUnchangedId(const MusicId &music_id) {
  return isEqualId(music_id, unchanged);
}

/**
 * Compares two music ids.
 * @param music_id a music id
 * @param other_music_id another music id
 * @return true if the ids are the same
 */
bool Music::isEqualId(const MusicId &music_id, const MusicId &other_music_id) {
  return music_id == other_music_id;
}

/**
 * Loads the file and plays the music repeatedly.
 * @return true if the music was loaded successfully, false otherwise
 */
bool Music::play(void) {

  bool success = false;

  if (is_initialized()) {

    SDL_RWops *rw = FileTools::data_file_open_rw(file_name);
    music = Mix_LoadMUS_RW(rw);
    FileTools::data_file_close_rw(rw);

    if (music == NULL) {
      std::cerr << "Unable to create music '" << file_name << "': " << Mix_GetError() << std::endl;
    }
    else {
      int result = Mix_PlayMusic(music, -1);

      if (result == -1) {
	std::cerr << "Unable to play music '" << file_name << "': " << Mix_GetError() << std::endl;
      }
      else {
	success = true;
      }
    }
  }

  return success;
}

/**
 * Stops playing the music.
 */
void Music::stop(void) {

  if (is_initialized()) {
    Mix_HaltMusic();
    music = NULL;
  }
}

/**
 * Returns whether the music is paused.
 * @return true if the music is paused, false otherwise
 */
bool Music::is_paused(void) {

  if (!is_initialized()) {
    return false;
  }

  return Mix_PausedMusic();
}

/**
 * Pauses or resumes the music.
 * @param pause true to pause the music, false to resume it
 */
void Music::set_paused(bool pause) {
  if (is_initialized()) {

    if (pause) {
      Mix_PauseMusic();
    }
    else {
      Mix_ResumeMusic();
    }
  }
}
