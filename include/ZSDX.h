/**
 * This module initializes the game engine and handles the screen.
 */

#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

#include "Common.h"
#include "TileAnimationManager.h"

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
  TileAnimationManager tile_animation_manager;

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

  bool handle_event(const SDL_Event &event);
  void set_fullscreen(bool fullscreen);
  bool is_fullscreen(void);
  void switch_fullscreen(void);

  /**
   * Returns the tile animation manager object.
   * This object handles the tile animations.
   */
  inline TileAnimationManager *get_tile_animation_manager() { return &tile_animation_manager; }
};

#endif
