/**
 * This module defines the class SpriteAnimations.
 */

#include "SpriteAnimations.h"
#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "FileTools.h"
#include <fstream>
#include <sstream>

/**
 * Loads the animations of a sprite from a file.
 * @param id id of the sprite (used to determine the sprite file)
 */
SpriteAnimations::SpriteAnimations(SpriteId id) {

  // read the file

  char file_name[256];
  sprintf(file_name, "sprites/%s.zsd", id.c_str());
  ifstream sprite_file(FileTools::data_file_add_prefix(file_name));

  if (!sprite_file) {
    cerr << "Cannot open file '" << FileTools::data_file_add_prefix(file_name) << "'" << endl;
    exit(1);
  }

  string line;

  SDL_Rect *positions_in_src;
  SpriteAnimationDirection **directions;
  string name, image_file_name;
  int nb_directions, nb_frames, x_hotspot, y_hotspot, loop_on_frame;
  Uint32 frame_delay;
  SDL_Surface *src_image;

  // read each animation
  while (std::getline(sprite_file, line)) {

    if (line.size() == 0) {
      continue;
    }

    // first line: animation info
    
    istringstream iss0(line);
    iss0 >> name >> image_file_name >> nb_directions >> nb_frames
	 >> x_hotspot >> y_hotspot >> frame_delay >> loop_on_frame;

    //    cout << "name: " << name << endl;

    image_file_name = "images/sprites/" + image_file_name;
    src_image = IMG_Load(FileTools::data_file_add_prefix(image_file_name.c_str()));

    directions = new SpriteAnimationDirection*[nb_directions];

    //    cout << "nb_directions = " << nb_directions << ", nb_frames = " << nb_frames << endl;

    for (int i = 0; i < nb_directions; i++) {

      positions_in_src = new SDL_Rect[nb_frames];

      for (int j = 0; j < nb_frames; j++) {

	do {
	  std::getline(sprite_file, line);
	}
	while (line.size() == 0);

	//	cout << "parsing direction " << i << " (frame " << j << ")\n";
	
	istringstream iss(line);
	iss >> positions_in_src[j].x >> positions_in_src[j].y
	    >> positions_in_src[j].w >> positions_in_src[j].h;
      }

      directions[i] = new SpriteAnimationDirection(nb_frames, positions_in_src);
    }

    animations[name] = new SpriteAnimation(src_image, nb_directions, directions,
					   x_hotspot, y_hotspot, frame_delay, loop_on_frame);

    // default animation
    if (animations.size() == 1) {
      default_animation = animations[name];
    }
  }
}

/**
 * Destructor.
 */
SpriteAnimations::~SpriteAnimations(void) {

  // delete the animations
  map<SpriteId, SpriteAnimation*>::const_iterator it;

  for (it = animations.begin(); it != animations.end(); it++) {
    delete it->second;
  }
}
