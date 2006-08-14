#include <SDL/SDL_image.h>
#include "link.h"

Link::Link(void) {
  link_stopped_image = IMG_Load("images/sprites/link/stopped.png");
  position_in_image.x = 72;
  position_in_image.y = 0;
  position_in_image.w = 24;
  position_in_image.h = 24;

  position_in_map.x = 0;
  position_in_map.y = 0;
}

Link::~Link(void) {
  SDL_FreeSurface(link_stopped_image);
}

void Link::display(SDL_Surface *surface) {
  SDL_BlitSurface(link_stopped_image, &position_in_image,
		  surface, &position_in_map);
}
