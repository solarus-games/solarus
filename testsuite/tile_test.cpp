#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "map.h"

void create_map(SDL_Surface *surface);
void destroy_map();

const int TILE_NUMBER = 2;
SDL_Surface *screen;
SDL_Surface *tileset;
SimpleTile *little_tree;
SimpleTile *grass;
Tile *tiles[TILE_NUMBER];
Map *map;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_VIDEO);
  
  //  SDL_Surface *screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  SDL_Surface *screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

  create_map(screen);

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
    }

    map->display(screen);
    SDL_Flip(screen);
  }
  destroy_map();
  SDL_Quit();
}

void create_map(SDL_Surface *surface) {
  map = new Map(320, 240,
		     SDL_MapRGB(surface->format, 128, 128, 128));

  // load the tileset
  tileset = IMG_Load("../images/tilesets/outside.png");

  // create the tile data
  {
    SDL_Rect where_in_tileset = {0, 32, 32, 32};
    little_tree = new SimpleTile(tileset, where_in_tileset, OBSTACLE);
  }

  {
    SDL_Rect where_in_tileset = {0, 0, 8, 8};
    grass = new ExtensibleTile(tileset, where_in_tileset, NO_OBSTACLE, 10, 2);
  }

  // create the tiles
  {
    SDL_Rect where_in_map = {32, 16};
    tiles[0] = new Tile(grass, where_in_map);
  }
  
  {
    SDL_Rect where_in_map = {64, 24};
    tiles[1] = new Tile(little_tree, where_in_map);
  }

  for (int i = 0; i < TILE_NUMBER; i++) {
    map->add_object(tiles[i]);
  }
}

void destroy_map(void) {
  SDL_FreeSurface(tileset);
  delete little_tree;

  for (int i = 0; i < TILE_NUMBER; i++) {
    delete tiles[i];
  }

  delete map;
}
