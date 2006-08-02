#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "main.h"
#include "map.h"
#include "global.h"

void create_map(void);
void destroy_map(void);

const int TILE_NUMBER = 3;
SDL_Surface *tileset;
SimpleTile *little_tree;
ExtensibleTile *grass;
AnimatedTile *little_flower;
Tile *tiles[TILE_NUMBER];
Map *map;

int main(int argc, char **argv) {
  zsdx_init();

  create_map();

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

    map->display(zsdx_global.screen);
    SDL_Flip(zsdx_global.screen);
  }
  destroy_map();
  zsdx_exit();
}

void create_map(void) {
  map = new Map(320, 240, SDL_MapRGB(zsdx_global.screen->format, 128, 128, 128));

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

  {
    SDL_Rect where_in_tileset[3] = {
      {0, 96, 16, 16}, // frame 1
      {16, 96, 16, 16}, // frame 2
      {32, 96, 16, 16}, // frame 3
    };
    
    little_flower = new AnimatedTile(tileset, where_in_tileset, ANIMATION_SEQUENCE_123, NO_OBSTACLE);
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

  {
    SDL_Rect where_in_map = {96, 24};
    tiles[2] = new Tile(little_flower, where_in_map);
  }

  for (int i = 0; i < TILE_NUMBER; i++) {
    map->add_object(tiles[i]);
  }
}

void destroy_map(void) {
  SDL_FreeSurface(tileset);
  delete little_tree;
  delete grass;
  delete little_flower;

  for (int i = 0; i < TILE_NUMBER; i++) {
    delete tiles[i];
  }

  delete map;
}
