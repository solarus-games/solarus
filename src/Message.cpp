#include "Message.h"
#include "DialogBox.h"
#include "FileTools.h"
#include "SDL/SDL_config_lib.h"

/**
 * Creates a new message.
 * @param dialog_box the dialog box
 * @param message_id id of the message
 */
Message::Message(DialogBox *dialog_box, MessageId message_id) {

  this->dialog_box = dialog_box;

  // open the file
  string file_name;
  if (message_id[0] == '_') {
    // message from the engine
    file_name = "text/engine.msg";
  }
  else {
    file_name = "text/game.msg";
  }
  file_name = FileTools::data_file_add_prefix(file_name);

  // parse the message
  CFG_File ini;

  if (CFG_OpenFile(file_name.c_str(), &ini) != CFG_OK) {
    DIE("Cannot load the message file '" << file_name << "': " << CFG_GetError());
  }

  if (CFG_SelectGroup(message_id.c_str(), 0) != CFG_OK) {
    DIE("Cannot find message '" << message_id << "'");
  }

  // text
  lines[0] = CFG_ReadText("line1", "");
  lines[1] = CFG_ReadText("line2", "");
  lines[2] = CFG_ReadText("line3", "");

  // icon
  int icon_number = CFG_ReadInt("icon", 0);
  if (icon_number != 0) {
    icon = NULL; // TODO
  }
  else {
    icon = NULL;
  }

  // question
  question = CFG_ReadBool("question", false);
  first_answer = true;

  // next message
  next_message_id = CFG_ReadText("next", "");
  next_message_id_2 = CFG_ReadText("next2", "");

  // cancel mode
  string cancel_mode_text = CFG_ReadText("cancel", "no");
  DialogCancelMode cancel_mode;
  if (cancel_mode_text == "current") {
    cancel_mode = DIALOG_CANCEL_NONE;
  }
  else if (cancel_mode_text == "all") {
    cancel_mode = DIALOG_CANCEL_CURRENT;
  }
  else {
    cancel_mode = DIALOG_CANCEL_NONE;
  }
  dialog_box->set_cancel_mode(cancel_mode);

  // close the file
  CFG_CloseFile(&ini);
}

/**
 * Destructor.
 */
Message::~Message(void) {
  if (icon != NULL) {
    SDL_FreeSurface(icon);
  }
}

/**
 * Returns the id of the next message to display, or
 * an empty string if this is the last message.
 * @return the if of the message to display when this one
 * is over
 */
MessageId Message::get_next_message_id(void) {

  if (question && !first_answer) {
    return next_message_id_2;
  }

  return next_message_id;
}

/**
 * Returns whether the message is now completely displayed.
 * @return true if the message is over
 */
bool Message::is_over(void) {
  return false;
}

/**
 * Updates the message.
 */
void Message::update(void) {

}

/**
 * Displays the message on a surface.
 */
void Message::display(SDL_Surface *destination_surface) {

}
