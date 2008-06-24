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
SpriteAnimations::SpriteAnimations(SpriteAnimationsId id) {

  // compute the file name
  string file_name = "sprites/" + id + ".zsd";

  // open the file
  ifstream sprite_file(FileTools::data_file_add_prefix(file_name).c_str());
  if (!sprite_file) {
    DIE("Cannot open file '" << FileTools::data_file_add_prefix(file_name) << "'");
  }

  // read the file
  string line;

  SDL_Rect *positions_in_src;
  SpriteAnimationDirection **directions;
  string name, image_file_name;
  int nb_directions, nb_frames, x_origin, y_origin, loop_on_frame;
  int x, y, width, height, rows, columns;
  Uint32 frame_delay;

  // read each animation
  while (std::getline(sprite_file, line)) {

    if (line.size() == 0) {
      continue;
    }

    // first line: animation info
    
    istringstream iss0(line);
    iss0 >> name >> image_file_name >> nb_directions
	 >> frame_delay >> loop_on_frame;

    directions = new SpriteAnimationDirection*[nb_directions];

    for (int i = 0; i < nb_directions; i++) {

      do {
	std::getline(sprite_file, line);
      }
      while (line.size() == 0);

      istringstream iss(line);
      iss >> x >> y >> width >> height >> x_origin >> y_origin
	  >> nb_frames >> columns;

      if (nb_frames % columns == 0) {
	rows = nb_frames / columns;
      }
      else {
	rows = (nb_frames / columns) + 1;
      }

      positions_in_src = new SDL_Rect[nb_frames];
      int j = 0; // frame number
      for (int r = 0; r < rows && j < nb_frames; r++) {	
	for (int c = 0; c < columns && j < nb_frames; c++) {

	  positions_in_src[j].x = x + c * width;
	  positions_in_src[j].y = y + r * height;
	  positions_in_src[j].w = width;
	  positions_in_src[j].h = height;

	  j++;
	}
      }

      directions[i] = new SpriteAnimationDirection(nb_frames, positions_in_src,
						   x_origin, y_origin);
    }

    animations[name] = new SpriteAnimation(image_file_name, nb_directions, directions,
					   frame_delay, loop_on_frame);

    // default animation
    if (animations.size() == 1) {
      default_animation_name = name;
    }
  }
}

/**
 * Destructor.
 */
SpriteAnimations::~SpriteAnimations(void) {

  // delete the animations
  map<SpriteAnimationsId, SpriteAnimation*>::const_iterator it;

  for (it = animations.begin(); it != animations.end(); it++) {
    delete it->second;
  }
}

/**
 * Returns an animation.
 * @param animation_name name of the animation to get
 * @return the specified animation
 */
SpriteAnimation * SpriteAnimations::get_animation(string animation_name) {
  return animations[animation_name];
}

/**
 * Returns the name of the default animation, i.e. the first one.
 * @return the name of the default animation
 */
string SpriteAnimations::get_default_animation(void) {
  return default_animation_name;
}
