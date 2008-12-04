#ifndef ZSDX_JUMP_MOVEMENT_H
#define ZSDX_JUMP_MOVEMENT_H

#include "Common.h"
#include "movements/PathMovement.h"

/**
 * Movement for an entity that jumps in a direction.
 */
class JumpMovement: public PathMovement {

 private:

  static const SDL_Rect basic_translations[8];
  SDL_Rect *translation_vectors;

 public:

  JumpMovement(int direction, int length);
  ~JumpMovement(void);

};

#endif
