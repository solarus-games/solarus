#include <SDL/SDL.h>
#include "sprite_animation_direction.h"

SpriteAnimationDirection::SpriteAnimationDirection(SDL_Surface *src_image, int nb_frames,
						   int loop_on_frame, SDL_Rect *frames):
  src_image(src_image), nb_frames(nb_frames), loop_on_frame(loop_on_frame), frames(frames) {

}

// simple constructor for only one frame
SpriteAnimationDirection::SpriteAnimationDirection(SDL_Surface *src_image, SDL_Rect &frame):
  src_image(src_image), nb_frames(1), loop_on_frame(-1) {
  frames = new SDL_Rect[1];
  frames[0] = frame;
}

SpriteAnimationDirection::~SpriteAnimationDirection(void) {
  delete[] frames;
}

int SpriteAnimationDirection::get_next_frame(int current_frame) {
  int next_frame = current_frame + 1;

  // if we are on the last frame
  if (next_frame == nb_frames) {
    // we loop on the appropriate frame
    next_frame = loop_on_frame;
  }

  return next_frame;
}

void SpriteAnimationDirection::display_on_map(Map *map, SDL_Rect &where_in_map, int current_frame) {
//   printf("trying to blit rect (%d,%d,%d,%d) at position (%d,%d,%d,%d)\n",
// 	 frames[current_frame].x, frames[current_frame].y, frames[current_frame].w, frames[current_frame].h,
// 	 where_in_map.x, where_in_map.y, where_in_map.w, where_in_map.h);
  SDL_BlitSurface(src_image, &frames[current_frame], map->get_surface(), &where_in_map);
}
