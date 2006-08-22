#include <SDL/SDL.h>
#include "sprite_animation_direction.h"

SpriteAnimationDirection::SpriteAnimationDirection(SDL_Surface *src_image, int nb_frames, SDL_Rect *frames):
  src_image(src_image), nb_frames(nb_frames), current_frame(0), frames(frames) {

}

// simple constructor for only one frame
SpriteAnimationDirection::SpriteAnimationDirection(SDL_Surface *src_image, SDL_Rect &frame):
  src_image(src_image), nb_frames(1), current_frame(0) {
  frames = new SDL_Rect[1];
  frames[0] = frame;
}

SpriteAnimationDirection::~SpriteAnimationDirection(void) {
  delete[] frames;
}

void SpriteAnimationDirection::next_frame(void) {
  current_frame = (current_frame + 1) % nb_frames;
}

void SpriteAnimationDirection::display_on_map(SDL_Surface *map, SDL_Rect &where_in_map) {
  SDL_BlitSurface(src_image, &frames[current_frame], map, &where_in_map);
}
