/*
 * Example of tileset written manually.
 * The tileset editor will generate classes like this one.
 */
#include "tileset.h"

class TilesetHouse: public Tileset {

 public:
  TilesetHouse(void);
  inline ~TilesetHouse(void) { }

  void load(void);
  TileData *get_tile(int index);
};
