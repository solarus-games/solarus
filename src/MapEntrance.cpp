/**
 * This module defines the class MapEntrance.
 */

#include "MapEntrance.h"

/**
 * Constructor.
 */
MapEntrance::MapEntrance(Layer layer, int link_x, int link_y, int link_direction):
  layer(layer), link_x(link_x), link_y(link_y), link_direction(link_direction) {

}

/**
 * Returns the layer of Link in this initial state.
 * @return the layer
 */
Layer MapEntrance::get_layer(void) {
  return layer;
}

/**
 * Returns the x coordinate of Link's position.
 */
int MapEntrance::get_link_x(void) {
  return link_x;
}

/**
 * Returns the y coordinate of Link's position.
 */
int MapEntrance::get_link_y(void) {
  return link_y;
}

/**
 * Returns Link's initial direction.
 */
int MapEntrance::get_link_direction(void) {
  return link_direction;
}

