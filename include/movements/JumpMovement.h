#ifndef ZSDX_JUMP_MOVEMENT_H
#define ZSDX_JUMP_MOVEMENT_H

#include "Common.h"
#include "movements/PixelMovement.h"

/**
 * Movement for an entity that jumps in a direction.
 */
class JumpMovement: public PixelMovement {

 private:

  static const SDL_Rect basic_translations[8];
  SDL_Rect *translation_vectors;
  int jump_height;

 protected:
  void make_next_move(void);

 public:

  JumpMovement(Map *map, int direction, int length, bool with_collisions);
  ~JumpMovement(void);

  int get_jump_height(void);
};

#endif
