/**
 * This module defines the class LinkAnimations.
 */

#include "LinkAnimations.h"
#include "SpriteAnimationDirection.h"
#include "FileTools.h"

/**
 * Constructor.
 */
LinkAnimations::LinkAnimations(void):
SpriteAnimations(2) {
  create_animations();
}

/**
 * Destructor.
 */
LinkAnimations::~LinkAnimations(void) {
  SDL_FreeSurface(link_stopped_image);
  SDL_FreeSurface(link_walking_image);
}

/**
 * Only instance of LinkAnimations.
 */
LinkAnimations *LinkAnimations::instance = NULL;

/**
 * Returns the only instance.
 */
LinkAnimations *LinkAnimations::get_instance(void) {
  if (instance == NULL) {
    instance = new LinkAnimations();
  }
  return instance;
}

/**
 * Destroys the instance.
 */
void LinkAnimations::destroy_instance(void) {
  delete instance;
  instance = NULL;
}

/**
 * Creates the animations. This function is called by
 * the constructor.
 */
void LinkAnimations::create_animations(void) {
  // create only green link for now
  SDL_Rect position_in_src;
  SDL_Rect *positions_in_src;
  SpriteAnimationDirection **directions;
  int i;

  link_stopped_image = IMG_Load(FileTools::data_file_add_prefix("images/sprites/link/stopped.png"));
  link_walking_image = IMG_Load(FileTools::data_file_add_prefix("images/sprites/link/walking.png"));

  // stopped
  directions = new SpriteAnimationDirection*[4];

  // right
  position_in_src.x = 0;
  position_in_src.y = 0;
  position_in_src.w = 24;
  position_in_src.h = 24;
  directions[0] = new SpriteAnimationDirection(link_stopped_image, position_in_src);

  // up
  position_in_src.x = 24;
  position_in_src.y = 0;
  position_in_src.w = 24;
  position_in_src.h = 24;
  directions[1] = new SpriteAnimationDirection(link_stopped_image, position_in_src);

  // left
  position_in_src.x = 48;
  position_in_src.y = 0;
  position_in_src.w = 24;
  position_in_src.h = 24;
  directions[2] = new SpriteAnimationDirection(link_stopped_image, position_in_src);

  // down
  position_in_src.x = 72;
  position_in_src.y = 0;
  position_in_src.w = 24;
  position_in_src.h = 24;
  directions[3] = new SpriteAnimationDirection(link_stopped_image, position_in_src);

  animations[0] = new SpriteAnimation(4, directions, 12, 23);

  // stopped
  directions = new SpriteAnimationDirection*[4];

  // right
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 24 * i;
    positions_in_src[i].y = 0;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[0] = new SpriteAnimationDirection(link_walking_image, 8, positions_in_src, 0);

  // up
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 192 + 24 * i;
    positions_in_src[i].y = 0;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[1] = new SpriteAnimationDirection(link_walking_image, 8, positions_in_src, 0);

  // left
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 24 * i;
    positions_in_src[i].y = 96;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[2] = new SpriteAnimationDirection(link_walking_image, 8, positions_in_src, 0);

  // down
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 192 + 24 * i;
    positions_in_src[i].y = 96;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[3] = new SpriteAnimationDirection(link_walking_image, 8, positions_in_src, 0);

  animations[1] = new SpriteAnimation(4, directions, 12, 31, 100);
}
