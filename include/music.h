#ifndef ZSDX_MUSIC_H
#define ZSDX_MUSIC_H

#include <fmod/fmod.h>

class Music {

 private:
  static bool initialized;
  FMUSIC_MODULE *module;
  
 public:
  char file_name[256];
  Music(const char *file_name);
  ~Music(void);

  static void initialize(void);
  static void exit(void);

  void play(void);
  void stop(void);
  bool isPaused(void);
  void setPaused(bool pause);
};

#endif
