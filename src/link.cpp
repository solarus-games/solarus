#include <SDL/SDL_image.h>
#include <cmath>
#include <iostream>
using namespace std;
#include "link.h"

Link::Link(void) {
  link_stopped_image = IMG_Load("images/sprites/link/stopped.png");
  position_in_image.x = 72;
  position_in_image.y = 0;
  position_in_image.w = 24;
  position_in_image.h = 24;

}

Link::~Link(void) {
  SDL_FreeSurface(link_stopped_image);
}

void Link::display_on_map(SDL_Surface *map) {
  update_position();
  SDL_BlitSurface(link_stopped_image, &position_in_image,
		  map, &where_in_map);
}
