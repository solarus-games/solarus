#ifndef ZSDX_GAME_H
#define ZSDX_GAME_H

/* This class represents the game.
 * I think that the saves will be handled here.
 */
class Game {

 private:

 public:
  /* Constructor
   */
  Game(void);

  /* Destructor
   */
  ~Game(void);
  
  /* Launch a map given its id
   */
  void launch_map(int map_id);
};

#endif
