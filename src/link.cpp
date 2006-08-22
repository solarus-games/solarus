#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "link.h"
#include "sprite_animation.h"

Link::Link(void):
Movable8Keyboard(16), AnimatedSprite(1) {
  create_animations();
}

Link::~Link(void) {
  SDL_FreeSurface(link_stopped_image);
}

void Link::display_on_map(SDL_Surface *map) {
  update_position();
  AnimatedSprite::display_on_map(map);
}

void Link::create_animations(void) {
  // only green link for now
  SDL_Rect where_in_src_image;
  SpriteAnimationDirection **directions;

  link_stopped_image = IMG_Load("images/sprites/link/stopped.png");

  // stopped
  directions = new SpriteAnimationDirection*[4];

  // right
  where_in_src_image.x = 0;
  where_in_src_image.y = 0;
  where_in_src_image.w = 24;
  where_in_src_image.h = 24;
  directions[0] = new SpriteAnimationDirection(link_stopped_image, where_in_src_image);

  // up
  where_in_src_image.x = 24;
  where_in_src_image.y = 0;
  where_in_src_image.w = 24;
  where_in_src_image.h = 24;
  directions[1] = new SpriteAnimationDirection(link_stopped_image, where_in_src_image);

  // left
  where_in_src_image.x = 48;
  where_in_src_image.y = 0;
  where_in_src_image.w = 24;
  where_in_src_image.h = 24;
  directions[2] = new SpriteAnimationDirection(link_stopped_image, where_in_src_image);

  // down
  where_in_src_image.x = 72;
  where_in_src_image.y = 0;
  where_in_src_image.w = 24;
  where_in_src_image.h = 24;
  directions[3] = new SpriteAnimationDirection(link_stopped_image, where_in_src_image);

  animations[0] = new SpriteAnimation(4, directions);
}

void Link::start_right(void) {
  Movable8Keyboard::start_right();
  animations[current_animation]->set_current_direction(0);
}

void Link::start_up(void) {
  Movable8Keyboard::start_up();
  animations[current_animation]->set_current_direction(1);
}

void Link::start_left(void) {
  Movable8Keyboard::start_left();
  animations[current_animation]->set_current_direction(2);
}

void Link::start_down(void) {
  Movable8Keyboard::start_down();
  animations[current_animation]->set_current_direction(3);
}
