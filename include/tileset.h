#include "tile_data.h" // Tile
#include "dynamic_array.h"

class Tileset {

 private:
  TileData *tiles[1024];
  int tile_number;
  
 public:
  ~Tileset(void);

  virtual void load(void) = 0;
  
  TileData *get_tile(int index) = 0;
};
