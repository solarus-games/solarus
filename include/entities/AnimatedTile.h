#ifndef ZSDX_ANIMATED_TILE_H
#define ZSDX_ANIMATED_TILE_H

#include "Common.h"
#include "Tile.h"

/**
 * Animated tile.
 * An animated tile has always three frames.
 */
class AnimatedTile: public Tile {

 public:

  /**
   * Tile animation sequence mode: 0-1-2 or 0-1-2-1.
   */
  enum AnimationSequence {
    ANIMATION_SEQUENCE_012  = 1,
    ANIMATION_SEQUENCE_0121 = 2,
  };

 private:

  // static variables to handle the animations of all tiles
  static int frame_counter;
  static int current_frames[3];
  static Uint32 next_frame_date;

  /**
   * Animation sequence type of this tile: 0-1-2-1 or 0-1-2.
   */
  const AnimationSequence sequence;

  /**
   * Array of 3 SDL_Rect representing the 3 animation frames
   * of this tile in the tileset image.
   * The 3 frames should have the same width and height.
   */
  SDL_Rect position_in_tileset[3];

 public:

  AnimatedTile(MapEntity::Obstacle obstacle, AnimationSequence sequence,
	       int width, int height, int x1, int y1, int x2, int y2, int x3, int y3);
  ~AnimatedTile(void);

  static void update(void);
  void display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image);

};

#endif
