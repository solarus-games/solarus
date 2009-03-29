#include "entities/Tile.h"
#include "entities/Tileset.h"
#include "entities/TilePattern.h"

/**
 * Creates a new tile on the map.
 * @param tile_pattern the tile pattern in the tileset
 * @param layer layer of the tile
 * @param x x position of the tile on the map
 * @param y y position of the tile on the map
 * @param width width of the tile (the pattern can be repeated)
 * @param height height of the tile (the pattern can be repeated)
 */
Tile::Tile(TilePattern *tile_pattern, Layer layer, int x, int y, int width, int height):
  MapEntity(layer, x, y, width, height), tile_pattern(tile_pattern) {

}

/**
 * Destructor.
 */
Tile::~Tile(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType Tile::get_type() {
  return TILE;
}

/**
 * Displays the tile on the map.
 */
void Tile::display_on_map(void) {
  tile_pattern->display_on_map(map, position_in_map);
}

/**
 * Returns the pattern of this tile.
 * @return the tile pattern
 */
TilePattern * Tile::get_tile_pattern(void) {
  return tile_pattern;
}
