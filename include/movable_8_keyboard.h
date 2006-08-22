#ifndef ZSDX_MOVABLE_8_KEYBOARD
#define ZSDX_MOVABLE_8_KEYBOARD

#include <SDL/SDL.h>
#include "movable.h"

class Movable8Keyboard: public Movable {

 private:
  bool started;
  int direction_mask;
  void update_movement(void);

 protected:
  int speed;

 public:
  Movable8Keyboard(int speed);
  virtual ~Movable8Keyboard(void) { }

  void start_right(void);
  void start_up(void);
  void start_left(void);
  void start_down(void);
  void stop_right(void);
  void stop_up(void);
  void stop_left(void);
  void stop_down(void);

};

#endif
