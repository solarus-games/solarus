#include <SDL/SDL.h>
#include "datatypes.h"
#include "map.h"
#include "map_object.h"
#include "sdl_user_events.h"
#include "global.h"
#include "dynamic_array.h"

Map::Map(int width, int height, zsdx_color_t background_color,
	 Tileset *tileset):
  width(width), height(height), background_color(background_color),
  objects(new DynamicArray<MapObject*>(256)), tileset(tileset) {

}

Map::~Map() {
  unload();
  delete objects;
}

void Map::unload(void) {
  for (int i = 0; i < objects->get_size(); i++) {
    delete objects->get(i);
  }
  objects->clear();
}

void Map::display(SDL_Surface *surface) {
  // for now we don't care about the scrolling

  // background color
  SDL_FillRect(surface, NULL, background_color);

  // map objects
  for (int i = 0; i < objects->get_size(); i++) {
    objects->get(i)->display_on_map(surface);
  }
  SDL_Flip(surface);
}

void Map::launch(void) {
  SDL_Event event;
  bool quit = false;

  while (!quit) {
    SDL_WaitEvent(&event);
    switch (event.type) {
    
    case SDL_QUIT:
      quit = true;
      break;
    
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
	quit = true;
	break;
      default:
	break;
      }
      break;
    
    case SDL_USEREVENT:
      switch (event.user.type) {
      case EVENT_ANIMATION_FRAME_FINISHED:
	// just redraw the screen
	break;
      }
      break;
    }
    display(zsdx_global.screen);
  }
}
