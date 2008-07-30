#include "Message.h"
#include "SDL/SDL_Config.h"

/**
 * Creates a new message.
 * @param message_id id of the message
 */
Message::Message(MessageId message_id) {

  // parse the message
  CFG_File config;

  if (CFG_OpenFile("todo", &config) != CFG_OK) {
    DIE("Cannot load the message file: " << CFG_GetError());
  }

  if (CFG_SelectGroup(message_id.c_str(), 0) != CFG_OK) {
    DIE("Cannot find message '" << message_id << "'");
  }

  CFG_CloseFile(&config);
}

/**
 * Destructor.
 */
Message::~Message(void) {

}
