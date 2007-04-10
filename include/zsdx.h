#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

#include <SDL/SDL.h>
#include "game_resource.h"
#include "tile_animation_manager.h"

/* Number of times the screen is redrawn is a second
 */
extern const int FRAMES_PER_SECOND;

/* Delay between two frames in millisecond
 * FRAME_DELAY = 1000 / FRAMES_PER_SECOND
 */
extern const int FRAME_DELAY;

/* Main class of the game engine
 */
class ZSDX {

 private:
  GameResource *game_resource;
  SDL_Surface *screen;
  TileAnimationManager tile_animation_manager;
  bool fullscreen;

 public:
  /* Initialize the game engine
   */
  ZSDX(void);

  /* Clean everything
   */
  ~ZSDX(void);

  void set_fullscreen(bool fullscreen);

  bool is_fullscreen(void);

  void switch_fullscreen(void);

  inline GameResource *get_game_resource(void) { return game_resource; }
  inline SDL_Surface *get_screen(void) { return screen; }
  inline TileAnimationManager *get_tile_animation_manager() { return &tile_animation_manager; }
};

/* Global variable to access ZSDX data
 */
extern ZSDX zsdx;

/* Global variable to access the game resources
 */
extern GameResource *game_resource;

#endif
