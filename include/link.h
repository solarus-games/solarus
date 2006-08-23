#ifndef ZSDX_LINK_H
#define ZSDX_LINK_H

#include <SDL/SDL.h>
#include "movable_8_keyboard.h"
#include "animated_sprite.h"

/* Link's sprite
 * It is animated and can be controlled with an 8 directions system
 */
class Link: public Movable8Keyboard, public AnimatedSprite {

 private:
  /* Image from which link's stopped animation is extracted
   */
  SDL_Surface *link_stopped_image;

 protected:
  /* Create Link's animations
   * This function is called by the constructor.
   */
  void create_animations(void);

 public:
  /* Constructor
   */
  Link(void);

  /* Destructor
   */
  ~Link(void);

  // redefinition of these methods from Movable8Keyboard
  // to take care of Link's animation direction 
  
  /* Function called when the player wants to go right
   */
  void start_right(void);

  /* Function called when the player wants to go upwards
   */
  void start_up(void);

  /* Function called when the player wants to go left
   */
  void start_left(void);

  /* Function called when the player wants to go downwards
   */
  void start_down(void);

  /* Display Link on the map with its current animation and
   * at its current position
   */
  void display_on_map(SDL_Surface *map);
};

#endif
