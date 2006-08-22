#ifndef ZSDX_SPRITE_ANIMATION_DIRECTION
#define ZSDX_SPRITE_ANIMATION_DIRECTION

#include <SDL/SDL.h>

class SpriteAnimationDirection {

 private:
  SDL_Surface *src_image;
  const int nb_frames;
  int current_frame;
  SDL_Rect *frames;

 public:
  SpriteAnimationDirection(SDL_Surface *src_image, int nb_frames, SDL_Rect *frames);
  SpriteAnimationDirection(SDL_Surface *src_image, SDL_Rect &frame);
  virtual ~SpriteAnimationDirection(void);

  void next_frame(void);
  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

#endif
