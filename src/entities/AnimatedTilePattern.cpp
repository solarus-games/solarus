#include "entities/AnimatedTilePattern.h"
#include "ZSDX.h"

/**
 * Interval in millisecond between two frames of an animation.
 */
static const Uint32 TILE_FRAME_INTERVAL = 250;

/**
 * Array to associate the current frame (0, 1 or 2) depending on
 * the sequence type and the frame counter (0 to 11).
 */
static const short frames[2][12] = {
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 0-1-2
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 0-1-2-1
};

/**
 * Frame counter (0 to 11), increased every 250 ms.
 */
int AnimatedTilePattern::frame_counter = 0;

/**
 * Current frame (0 to 2) for both sequences.
 */
int AnimatedTilePattern::current_frames[3] = {0, 0, 0};

/**
 * Date of the next frame change.
 */
Uint32 AnimatedTilePattern::next_frame_date;

/**
 * Constructor.
 * @param obstacle is the tile pattern an obstacle?
 * @param sequence animation sequence type
 * @param width width of the tile (the same for each frame of the animation)
 * @param height height of the tile (the same for each frame of the animation)
 * @param x1 x position of the first frame in the tileset
 * @param y1 y position of the first frame in the tileset
 * @param x2 x position of the second frame in the tileset
 * @param y2 y position of the second frame in the tileset
 * @param x3 x position of the third frame in the tileset
 * @param y3 y position of the third frame in the tileset
 */
AnimatedTilePattern::AnimatedTilePattern(MapEntity::Obstacle obstacle,
					 AnimationSequence sequence,
					 int width, int height,
					 int x1, int y1,
					 int x2, int y2,
					 int x3, int y3):
  TilePattern(obstacle, width, height), sequence(sequence) {

  this->position_in_tileset[0].x = x1;
  this->position_in_tileset[0].y = y1;
  this->position_in_tileset[1].x = x2;
  this->position_in_tileset[1].y = y2;
  this->position_in_tileset[2].x = x3;
  this->position_in_tileset[2].y = y3;

  for (int i = 0; i < 3; i++) {
    this->position_in_tileset[i].w = width;
    this->position_in_tileset[i].h = height;
  }
}

/**
 * Destructor.
 */
AnimatedTilePattern::~AnimatedTilePattern(void) {
  
}

/**
 * Updates the current frame of all tiles.
 * This function is called repeatedly by the map.
 */
void AnimatedTilePattern::update(void) {

  Uint32 now = SDL_GetTicks();

  while (now >= next_frame_date) {

    frame_counter = (frame_counter + 1) % 12;
    current_frames[1] = frames[0][frame_counter];
    current_frames[2] = frames[1][frame_counter];

    next_frame_date += TILE_FRAME_INTERVAL; // the frame changes every 250 ms
  }
}

/**
 * Displays the tile on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset_image the tileset image of this tile pattern
 */
void AnimatedTilePattern::display(SDL_Surface *destination, const SDL_Rect &dst_position, SDL_Surface *tileset_image) {

  SDL_Rect dst = dst_position; // make a copy because the rectangle might be modified
  SDL_BlitSurface(tileset_image, &position_in_tileset[current_frames[sequence]],
		  destination, &dst);
}
