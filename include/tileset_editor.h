#ifndef ZSDX_TILESET_EDITOR_H
#define ZSDX_TILESET_EDITOR_H

#include <SDL/SDL.h>

/*
 * This class offer fonctionnalities to edit the properties of a tileset.
 * This properties are stored in a configuration file.
 */
class TilesetEditor {

 private:
  char *image_file; // image file of the tileset
  char *config_file; // configuration file of the tileset  

 public:
  TilesetEditor(void);
  ~TilesetEditor(void);

  void set_file(const char *file);
  void launch(SDL_Surface *src_surface);

};

#endif
