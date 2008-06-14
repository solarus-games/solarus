#ifndef ZSDX_MOVEMENT_FALLING_H
#define ZSDX_MOVEMENT_FALLING_H

/**
 * Movement of an item that falls on the floor and
 * bounces one or several times before it stops.
 * This movement is typically used by the pickable items
 * (rupees, hearts...) that appear when the player kill and enemy.
 */
class MovementFalling: public Movement {

 public:

  MovementFalling(int height);
  ~MovementFalling(void);

};

#endif
