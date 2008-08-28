#ifndef ZSDX_KEYS_EFFECT_H
#define ZSDX_KEYS_EFFECT_H

#include "Common.h"

/**
 * What happens when the user presses the Action key.
 */
enum ActionKeyEffect {
  ACTION_KEY_NONE,     /**< nothing happens when the user presses the Action key */

  ACTION_KEY_VALIDATE, /**< validate a choice */
  ACTION_KEY_NEXT,     /**< display the next message in a dialog */
  ACTION_KEY_INFOS,    /**< display some information about the selected item in the menu */
  ACTION_KEY_RETURN,   /**< exit a dialog or a submenu */

  ACTION_KEY_CHECK,    /**< look at something */
  ACTION_KEY_OPEN,     /**< open a door or a chest */
  ACTION_KEY_ACTION,   /**< perform an action */
  ACTION_KEY_LIFT,     /**< lift an objet */
  ACTION_KEY_THROW,    /**< throw an object */
  ACTION_KEY_GRAB,     /**< grab an object */
  ACTION_KEY_STOP,     /**< stop an action (for example: using an object which consumes magic) */
  ACTION_KEY_SPEAK,    /**< speak with an NPC */
};

/**
 * What happens when the user presses the Sword key.
 */
enum SwordKeyEffect {
  SWORD_KEY_NONE   = 0,    /**< nothing happens when the user presses the Sword key */
  SWORD_KEY_SAVE   = 1,    /**< save the game */
  SWORD_KEY_RETURN = 2,    /**< exit a submenu */
  SWORD_KEY_CHOOSE = 3,    /**< choose a letter when typing the player's name in the selection menu */
  SWORD_KEY_SKIP   = 4,    /**< skip a message in a dialog */
  SWORD_KEY_SWORD  = 5,    /**< swing the sword (this is the case most of the time) */
};

/**
 * What happens when the user presses the Pause key.
 */
enum PauseKeyEffect {
  PAUSE_KEY_NONE,      /**< nothing happens when the user presses the Pause key */
  PAUSE_KEY_PAUSE,     /**< open the pause menu */
  PAUSE_KEY_RETURN,    /**< close the pause menu */
};

/**
 * This class associates to each game key (the action key,
 * the sword key, the pause key, and the two item keys)
 * the current effect of this key.
 *
 * When the user presses one of these keys, the engine calls
 * this class to determine what should happen.
 * This class is also used by the HUD to display the 5 icons
 * corresponding to these keys.
 */
class KeysEffect {

 private:

  ActionKeyEffect action_key_effect;       // current effect of the action key
  ActionKeyEffect action_key_effect_saved; // a previous effect of the action key
  bool action_key_enabled;

  SwordKeyEffect sword_key_effect;         // current effect of the sword key
  SwordKeyEffect sword_key_effect_saved;   // a previous effect of the sword key
  bool sword_key_enabled;

  PauseKeyEffect pause_key_effect; // effect of the pause key
  bool pause_key_enabled;

  bool item_keys_enabled;

 public:

  KeysEffect(void);
  ~KeysEffect(void);

  // action key

  ActionKeyEffect get_action_key_effect(void);
  void set_action_key_effect(ActionKeyEffect action_key_effect);
  bool is_action_key_enabled(void);
  void set_action_key_enabled(bool enable);
  void save_action_key_effect(void);
  void restore_action_key_effect(void);

  // sword key

  SwordKeyEffect get_sword_key_effect(void);
  void set_sword_key_effect(SwordKeyEffect sword_key_effect);
  bool is_sword_key_enabled(void);
  void set_sword_key_enabled(bool enable);
  void save_sword_key_effect(void);
  void restore_sword_key_effect(void);

  // pause key

  PauseKeyEffect get_pause_key_effect(void);
  void set_pause_key_effect(PauseKeyEffect pause_key_effect);
  bool is_pause_key_enabled(void);
  void set_pause_key_enabled(bool enable);

  // item keys
  bool are_item_keys_enabled(void);
  void set_item_keys_enabled(bool enable);

  // the 5 keys
  void set_all_keys_enabled(bool enable);

};

#endif
