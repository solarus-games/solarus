#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <stdlib.h>
using namespace std;
#include <iostream>
#include "music.h"

bool Music::initialized = false;

Music::Music(const char *file_name):
  module(NULL) {
  //  this->file_name = new char[strlen(file_name) + 7];
  sprintf(this->file_name, "music/%s", file_name);
}

Music::~Music(void) {
}

void Music::initialize(void) {
  if (FSOUND_Init(44100, 32, 0)) {
    initialized = true;
  }
  else {
    // try with ESD, for Linux
    FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
    if (FSOUND_Init(44100, 32, 0)) {
      initialized = true;
    }
    else {
      cerr << "Unable to initialize FMOD: " << FMOD_ErrorString(FSOUND_GetError()) << '\n';
    }
  }
}

void Music::exit(void) {
  if (initialized) {
    FSOUND_Close();
  }
}

void Music::play(void) {
  if (initialized) {
    module = FMUSIC_LoadSong(file_name);
    if (module == NULL) {
      cerr << "Unable to play music: " << FMOD_ErrorString(FSOUND_GetError()) << '\n';
    }
    else {
      FMUSIC_PlaySong(module);
    }
  }
}

void Music::stop(void) {
  if (initialized) {
    FMUSIC_StopSong(module);
    FMUSIC_FreeSong(module);
  }
}

bool Music::isPaused(void) {
  if (initialized) {
    return FMUSIC_GetPaused(module);
  }
  else {
    return false;
  }
}

void Music::setPaused(bool pause) {
  if (initialized) {
    FMUSIC_SetPaused(module, pause);
  }
}
