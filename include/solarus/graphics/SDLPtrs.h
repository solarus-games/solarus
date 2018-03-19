#pragma once

#include <SDL_render.h>
#include <memory>

struct SDL_Texture_Deleter {
    void operator()(SDL_Texture* texture) const {
      SDL_DestroyTexture(texture);
    }
};
using SDL_Texture_UniquePtr = std::unique_ptr<SDL_Texture, SDL_Texture_Deleter>;

struct SDL_Surface_Deleter {
     void operator()(SDL_Surface* sdl_surface) const {
       SDL_FreeSurface(sdl_surface);
     }
 };
 using SDL_Surface_UniquePtr = std::unique_ptr<SDL_Surface, SDL_Surface_Deleter>;
