#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

#include "Common.h"

/**
 * Number of times the screen in redrawn is a second.
 */
extern const int FRAMES_PER_SECOND;

/**
 * Delay between two frames in millisecond
 * FRAME_DELAY = 1000 / FRAMES_PER_SECOND
 */
extern const int FRAME_INTERVAL;

/**
 * Only instance.
 */
extern ZSDX *zsdx;

/**
 * Main class of the game engine. It contains objects to
 * initialize the game and handles the screen.
 */
class ZSDX {

 private:

  bool fullscreen;

  void launch_adventure_mode(Savegame *savegame);
  void launch_solarus_dreams_mode(Savegame *savegame);

 public:

  ZSDX(void);
  ~ZSDX(void);

  void main(void);

  SDL_Surface *screen;
  TTF_Font *font;
  GameResource *game_resource;
  Game *game;

  bool exiting;

  void handle_event(const SDL_Event &event);
  void set_fullscreen(bool fullscreen);
  bool is_fullscreen(void);
  void switch_fullscreen(void);

  bool is_exiting(void);
  void set_exiting(void);
};

#endif
