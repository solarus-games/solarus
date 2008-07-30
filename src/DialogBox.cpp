#include "DialogBox.h"
#include "Message.h"
#include "FileTools.h"
#include "Sprite.h"

SDL_Surface * DialogBox::img_box = NULL;

static SDL_Rect box_src_position = {0, 0, 220, 60};
static SDL_Rect box_dst_position = {51, 144, 0, 0};

/**
 * Creates a new dialog box.
 * @param first_message_id id of the message to show
 * (it can be followed by other messages)
 */
DialogBox::DialogBox(MessageId first_message_id) {

  // first instance: load the image
  if (img_box == NULL) {
    img_box = FileTools::open_image("hud/dialog_box.png");
  }

  // create the sprite
  sprite_message_end_arrow = new Sprite("hud/dialog_box_message_end");

  current_message = new Message(first_message_id);
}

/**
 * Destructor.
 */
DialogBox::~DialogBox(void) {
  delete sprite_message_end_arrow;
}

/**
 * Updates the dialog box.
 * This function is called repeatedly by the game
 * when the dialog box exists.
 */
void DialogBox::update(void) {

}

/**
 * Displays the dialog box on a surface.
 * @param destination_surface the surface
 */
void DialogBox::display(SDL_Surface *destination_surface) {

  // display the dialog box
  SDL_BlitSurface(img_box, &box_src_position, destination_surface, &box_dst_position);

  // display the message
}
