/**
 * This module defines the class SpriteAnimations.
 */

#include "SpriteAnimations.h"
#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "FileTools.h"
#include <fstream>

/**
 * Loads the animations of a sprite from a file.
 * @param id id of the sprite (used to determine the sprite file)
 */
SpriteAnimations::SpriteAnimations(SpriteId id) {

  // read the file

  char file_name[256];
  sprintf(file_name, "sprites/%s.zsd", id.c_str());
  ifstream sprite_file(FileTools::data_file_add_prefix(file_name));

  if (!sprite_file) {
    cerr << "Cannot open file '" << FileTools::data_file_add_prefix(file_name) << "'" << endl;
    exit(1);
  }

  string line;

  // read each animation
  //  while (std::getline(sprite_file, line)) {

    // first line: animation info

  // don't forget default_animation
  //  }

  // TODO: replace below by parsing the file!

  
  // green link for now
  SDL_Rect *positions_in_src;
  SpriteAnimationDirection **directions;
  int i;

  SDL_Surface *link_stopped_image = IMG_Load(FileTools::data_file_add_prefix("images/sprites/link/stopped.png"));
  SDL_Surface *link_walking_image = IMG_Load(FileTools::data_file_add_prefix("images/sprites/link/walking.png"));

  // stopped
  directions = new SpriteAnimationDirection*[4];

  // right
  positions_in_src = new SDL_Rect[1];
  positions_in_src[0].x = 0;
  positions_in_src[0].y = 0;
  positions_in_src[0].w = 24;
  positions_in_src[0].h = 24;
  directions[0] = new SpriteAnimationDirection(1, positions_in_src);

  // up
  positions_in_src = new SDL_Rect[1];
  positions_in_src[0].x = 24;
  positions_in_src[0].y = 0;
  positions_in_src[0].w = 24;
  positions_in_src[0].h = 24;
  directions[1] = new SpriteAnimationDirection(1, positions_in_src);

  // left
  positions_in_src = new SDL_Rect[1];
  positions_in_src[0].x = 48;
  positions_in_src[0].y = 0;
  positions_in_src[0].w = 24;
  positions_in_src[0].h = 24;
  directions[2] = new SpriteAnimationDirection(1, positions_in_src);

  // down
  positions_in_src = new SDL_Rect[1];
  positions_in_src[0].x = 72;
  positions_in_src[0].y = 0;
  positions_in_src[0].w = 24;
  positions_in_src[0].h = 24;
  directions[3] = new SpriteAnimationDirection(1, positions_in_src);

  animations["stopped"] = new SpriteAnimation(link_stopped_image, 4, directions, 12, 23, 0, -1);
  default_animation = animations["stopped"];

  // walking
  directions = new SpriteAnimationDirection*[4];

  // right
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 24 * i;
    positions_in_src[i].y = 0;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[0] = new SpriteAnimationDirection(8, positions_in_src);

  // up
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 192 + 24 * i;
    positions_in_src[i].y = 0;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[1] = new SpriteAnimationDirection(8, positions_in_src);

  // left
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 24 * i;
    positions_in_src[i].y = 96;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[2] = new SpriteAnimationDirection(8, positions_in_src);

  // down
  positions_in_src = new SDL_Rect[8];
  for (i = 0; i < 8; i++) {
    positions_in_src[i].x = 192 + 24 * i;
    positions_in_src[i].y = 96;
    positions_in_src[i].w = 24;
    positions_in_src[i].h = 32;
  }
  directions[3] = new SpriteAnimationDirection(8, positions_in_src);

  animations["walking"] = new SpriteAnimation(link_walking_image, 4, directions, 12, 31, 100, 0);
}

/**
 * Destructor.
 */
SpriteAnimations::~SpriteAnimations(void) {

  // delete the animations
  map<SpriteId, SpriteAnimation*>::const_iterator it;

  for (it = animations.begin(); it != animations.end(); it++) {
    delete it->second;
  }
}
