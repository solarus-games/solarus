using namespace std;
#include <iostream>
#include <string>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "tileset_editor.h"

/**
 * Each tileset is defined by an image file, a tileset file and a C++ file.
 * The C++ file creates all the tiles of the tileset, and these tiles are used
 * by the maps. The C++ file can be written manually or generated 
 * by this tileset editor.
 *
 * The tileset file is the list of the tiles with all properties:
 * - coordinates in the tileset image file
 * - size
 * - obstacle
 * - layer (TODO)
 * - type: simple or animated
 * - for an animated tile: the coordinates and the sequence type
 */

/**
 * Creates a tileset editor.
 * @param tileset_name name of the tileset to edit
 * (if the corresponding file doesn't exist, this will be a new tileset)
 * This name is used to compute the tileset file name, the image file name
 * and the name of the C++ file generated.
 */
TilesetEditor::TilesetEditor(const char *tileset_name) {
  this->nb_tiles = 0;

  if (strlen(tileset_name) > MAX_TILESET_NAME) {
    cerr << "Tileset name too long\n";
    exit(1);
  }

  // tileset file
  sprintf(tileset_file_name, "src/tilesets/%s.zsd", tileset_name);

  // image file
  sprintf(image_file_name, "images/tilesets/%s.png", tileset_name);

  // C++ source file
  sprintf(source_file_name, "src/tilesets/%s.cpp", tileset_name);

  // C++ header file
  sprintf(header_file_name, "include/tilesets/%s.h", tileset_name);
}

/**
 * Destructor.
 */
TilesetEditor::~TilesetEditor(void) {
  for (int i = 0; i < nb_tiles; i++) {
    delete tiles[i];
  }
}

/**
 * Launches the editor and displays it on an SDL surface.
 * @param src_surface surface of the editor
 */
void TilesetEditor::launch(SDL_Surface *src_surface) {
  // create the tileset surface
  SDL_Surface *tileset_surface = IMG_Load(image_file_name);

  if (tileset_surface == NULL) {
    cerr << "Unable to load tileset " << image_file_name << endl;
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

/**
 * Saves the tileset file.
 */
void save(void) {
  // TODO
}
  
/**
 * Generates the C++ files.
 */
void generate_code(void) {
  // TODO
}

/**
 * Main function.
 * Usage: ./bin/tileset_editor tileset_name
 */
int main(int argc, char **argv) {

  // check the parameter
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " tileset_name\n";
    exit(1);
  }

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_Surface *screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

  TilesetEditor *editor = new TilesetEditor(argv[1]);
  editor->launch(screen);

  delete editor;
  SDL_Quit();
}
