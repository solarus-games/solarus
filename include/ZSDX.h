#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

#include "Common.h"
#include "VideoManager.h"

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
 
  /**
   * The current screen displayed: the title screen, the selection menu, the game, etc.
   */
  Screen *current_screen;
  SDL_Surface *root_surface;  /**< the surface where everything is drawn (always 320*240) */
  VideoManager *video_manager;
  bool exiting;

  void launch_adventure_mode(Savegame *savegame);
  void launch_solarus_dreams_mode(Savegame *savegame);

  void handle_event(const SDL_Event &event);
  void display(void);

  bool is_exiting(void);

 public:

  TTF_Font *font;
  Game *game;

  ZSDX(void);
  ~ZSDX(void);

  void main(void);

  VideoManager *get_video_manager(void);

  void set_game(Game *game);
  void set_exiting(void);

};

#endif
