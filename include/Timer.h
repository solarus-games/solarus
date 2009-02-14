#ifndef ZSDX_TIMER_H
#define ZSDX_TIMER_H

#include "Common.h"

/**
 * Represents a timer that a map script can start.
 */
class Timer {

 private:

  // timer
  const char *callback_name;     /**< name of the script function to call after the delay */

  Uint32 expiration_date;        /**< date when the timer is finished */
  bool finished;                 /**< indicates that the timer is finished */

  bool suspended;                /**< indicates whether the timer is suspended */
  Uint32 when_suspended;         /**< indicates when the timer was suspended */

  // sound
  Uint32 next_sound_date;        /**< date when the next countdown sound effect will be played */
  Sound *countdown_sound;        /**< sound played every second while the timer is running */
  Sound *countdown_hurry_sound;  /**< sound played when the timer is close to finish */

 public:

  Timer(Uint32 duration, const char *callback_name, bool with_sound);
  ~Timer(void);

  const char *get_callback_name(void);
  bool is_finished(void);

  void update(void);
  void set_suspended(bool suspended);
};

#endif
