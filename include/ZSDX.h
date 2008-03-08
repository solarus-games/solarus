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
 * Main class of the game engine. It contains objects to
 * initialize the game and handles the screen.
 */
class ZSDX {

 private:

  static bool fullscreen;
  static TileAnimationManager tile_animation_manager;

  static void initialize(void);
  static void exit(void);  
  static bool show_title_screen(void);
  static void show_game_file_selection(void);
  static void launch_adventure_mode(Savegame *savegame);
  static void launch_solarus_dreams_mode(Savegame *savegame);

 public:

  static void main(void);

  static SDL_Surface *screen;
  static GameResource *game_resource;
  static Game *game;

  static bool handle_event(const SDL_Event &event);
  static void set_fullscreen(bool fullscreen);
  static bool is_fullscreen(void);
  static void switch_fullscreen(void);

  /**
   * Returns the tile animation manager object.
   * This object handles the tile animations.
   */
  static inline TileAnimationManager *get_tile_animation_manager() { return &tile_animation_manager; }
};

#endif
