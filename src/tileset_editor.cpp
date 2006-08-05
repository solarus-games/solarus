#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <string>
#include "tileset_editor.h"

/*
 * Each tileset is defined by an image file and a C file. The C file
 * creates all the tiles of the tileset, and these tiles are used
 * by the maps. The C file can be written manually or generated 
 * by this tileset editor.
 *
 * The configuration file of a tileset is the list of the tiles with all properties:
 * - coordinates in the tileset image file
 * - size
 * - obstacle
 * - layer (TODO)
 * - type: simple, extensible or animated
 * - additionnal properties for extensible and animated tiles
 */

TilesetEditor::TilesetEditor(void):
  image_file(NULL), config_file(NULL) {

}

TilesetEditor::~TilesetEditor(void) {
  delete [] image_file;
  delete [] config_file;
}

void TilesetEditor::set_file(const char *file) {
  int length = strlen(file) + 1;
  image_file = new char[length];
  strcpy(image_file, file);

  config_file = new char[length + 4];
  sprintf(config_file, "%s.zsd", config_file);
}

void TilesetEditor::launch(SDL_Surface *src_surface) {
  // create the tileset surface
  SDL_Surface *tileset_surface = IMG_Load(image_file);

  if (tileset_surface == NULL) {
    std::cerr << "Unable to load tileset " << image_file << std::endl;
    return;
  }

  // main loop
  SDL_Event event;
  bool cont = true;

  while (cont) {
    SDL_WaitEvent(&event);

    switch (event.type) {

    case SDL_QUIT:
      cont = false;
      break;

    }

    SDL_BlitSurface(tileset_surface, NULL, src_surface, NULL);
    SDL_Flip(src_surface);
  }
}
