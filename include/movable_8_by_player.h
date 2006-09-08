#ifndef ZSDX_MOVABLE_8_KEYBOARD
#define ZSDX_MOVABLE_8_KEYBOARD

#include <SDL/SDL.h>
#include "movable.h"

/* Abstract class for a moving entity with the 8 basic directions
 * This class is designed for a sprite controlled by the keyboard
 * but can be also used to control the entity with a joypad.
 */
class Movable8ByPlayer: public Movable {

 protected:
  /* True if the entity is moving
   */
  bool started;

  /* Which ones of the 4 arrows are pressed by the player on the
   * keyboard
   */
  short direction_mask;

  /* Update the movement of the entity
   * This function is called when an arrow is pressed or released
   * on the keyboard
   */
  virtual void update_movement(void);

  /* Speed of the entity when it is moving
   */
  int speed;

 public:
  /* Constructor
   */
  Movable8ByPlayer(int speed);

  /* Destructor
   */
  virtual ~Movable8ByPlayer(void) { }

  /* Function called when the used presses the right arrow
   */
  void start_right(void);

  /* Function called when the used presses the up arrow
   */
  void start_up(void);

  /* Function called when the used presses the left arrow
   */
  void start_left(void);

  /* Function called when the used presses the down arrow
   */
  void start_down(void);

  /* Function called when the used releases the right arrow
   */
  void stop_right(void);

  /* Function called when the used releases the up arrow
   */
  void stop_up(void);

  /* Function called when the used releases the left arrow
   */
  void stop_left(void);

  /* Function called when the used releases the down arrow
   */
  void stop_down(void);
};

#endif
