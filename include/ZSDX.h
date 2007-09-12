/**
 * This module initializes the game engine and handles the screen.
 */

#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

using namespace std;
#include <iostream>
#include <SDL/SDL.h>
#include "GameResource.h"
#include "TileAnimationManager.h"
#include "Game.h"

#define MIN(x,y) ((x > y) ? (y) : (x))
#define MAX(x,y) ((x > y) ? (x) : (y))

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
 * initialize the game engine and handles the screen.
 */
class ZSDX {

 private:

  static SDL_Surface *screen;
  static bool fullscreen;
  static TileAnimationManager tile_animation_manager;

  /**
   * Initializes the game engine.
   */
  static void initialize(void);

  /**
   * Cleans everything.
   */
  static void exit(void);

 public:

  /**
   * Launches the game.
   */
  static void main(void);

  /**
   * The game resource object.
   * It contains the whole game database: the maps, the tilesets,
   * the sprites, the musics, etc.
   */
  static GameResource *game_resource;

  /**
   * The game object.
   * It provides all game functions to the maps.
   */
  static Game *game;

  /**
   * Sets the full screen mode or the windowed mode.
   * @param fullscreen true for full screen mode, false for the windowed mode
   */
  static void set_fullscreen(bool fullscreen);

  /**
   * Returns whether the game is in full screen.
   * @return true if the game is in full screen mode, false otherwise
   */
  static bool is_fullscreen(void);
  
  /**
   * Switches between full screen mode and windowed mode.
   */
  static void switch_fullscreen(void);

  /**
   * Returns the screen.
   * @return the screen
   */
  static inline SDL_Surface *get_screen(void) { return screen; }

  /**
   * Returns the tile animation manager object.
   * This object handles the tile animations.
   */
  static inline TileAnimationManager *get_tile_animation_manager() { return &tile_animation_manager; }
};

#endif
