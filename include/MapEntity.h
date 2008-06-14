#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include "Common.h"
#include "Layer.h"
#include <vector>

/**
 * Abstract class for all objects attached to a map: tiles,
 * enemies, Link, interactive objects, etc.
 * Each entity has:
 * - a layer on the map
 * - a position on the map (a rectangle),
 * - an origin point, relative to the rectangle's top-left corner
 * - 
 */
class MapEntity {

 protected:

  // position (mandatory for all kinds of entities)

  /**
   * Layer of the entity: LAYER_LOW, LAYER_INTERMEDIATE or LAYER_HIGH.
   * The layer is constant for the tiles and can change for Link and the enemies.
   */
  Layer layer;

  /**
   * Position of the entity on the map. The position is defined as a rectangle.
   * This rectangle represents the position of the entity of the map, it is
   * used for the collision tests. It can be different from the sprite's
   * rectangle of the entity.
   * For example, Link's position is a 16*16 rectangle, but its sprite may be
   * a 16*24 rectangle.
   */
  SDL_Rect position_in_map;

  /**
   * Coordinates of the origin point of the entity,
   * relative to the top-left corner of its rectangle.
   * Remember that when you call get_x() and get_y(), you get the coordinates
   * of the origin point on the map, not the coordinates of the rectangle's
   * top-left corner.
   * This is useful because the top-left corner of the entity's rectangle does
   * not represent the actual entity's coordinates.
   */
  SDL_Rect origin;

  // other data, used for some kinds of entities

  /**
   * Name of the entity (not used for all kinds of entities).
   * The name identifies the entity in the game.
   * An empty string indicates that the entity has no name.
   */
  string name;

  /**
   * Direction of the entity (not used for all kinds of entities).
   * If the entity has a sprite, this direction is considered to be
   * the direction of animation of the sprite.
   */
  int direction;

  /**
   * Sprite(s) representing the entity, not used for all kinds of entities because
   * some of them are invisible, and some of them handle their sprites themselves.
   */
  vector<Sprite*> *sprites;

  /**
   * Movement of the entity (not used for all kinds of entities).
   * NULL indicates that the entity has no movement.
   */
  Movement *movement;

  MapEntity(void);
  MapEntity(Layer layer, int x, int y, int width, int height);
  MapEntity(string name, int direction, Layer layer, int x, int y, int width, int height);

  void set_size(int width, int height);
  void set_origin(int x, int y);
  void add_sprite(SpriteAnimationsId id);
  void set_movement(Movement *movement);
  void clear_movement(void);

 public:

  virtual ~MapEntity(void);

  int get_x(void);
  int get_y(void);
  void set_x(int x);
  void set_y(int y);
  int get_width(void);
  int get_height(void);

  string get_name(void);
  int get_direction(void);
  Layer get_layer(void);
  const SDL_Rect * get_position_in_map(void);

  Sprite * get_sprite(int index);

  Movement * get_movement(void);
  virtual void movement_just_changed(void);

  bool overlaps(const SDL_Rect *rectangle);
  bool is_origin_point_in(const SDL_Rect *rectangle);

  virtual void update();
  virtual void display_on_map(Map *map);
};

#endif
