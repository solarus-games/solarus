#ifndef ZSDX_H
#define ZSDX_H

/* A map is represented by:
 * - a background color
 * - some tiles
 * - enemies and characters
 * We need the following types of tiles:
 * - single tile
 * - extensible tile
 * - animated tile
 * The coordinates cannot be stored in class Tile because tiles can be shared on the map.
 * Handle oblique obstacles.
 * 2 layers of tiles? or even more? make layer handling global (with swags? or is the layer of an object constant?)
 */
class Map {

 private:
  const int pixel_width;
  const int pixel_height;
  const int tile_number; // (pixel_width / TILE_SIZE) * (pixel_height / TILE_SIZE)
  Tile **tiles;
  GraphicObject **objects;

 public:
  Map(int pixel_width, int pixel_height);
  ~Map();

  void set_tile(int index, Tile *tile);
}

#endif
