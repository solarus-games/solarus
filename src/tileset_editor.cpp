using namespace std;
#include <iostream>
#include <string>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h> 
#include "tileset_editor.h"
#include "color.h"

/**
 * Each tileset is defined by an image file, a tileset file and a C++ file.
 * The C++ file creates all the tiles of the tileset, and these tiles are used
 * by the maps. The C++ file can be written manually or generated 
 * by this tileset editor.
 *
 * The tileset file is the list of the tiles with all properties:
 * - coordinates in the tileset image file
 * - size
 * - obstacle
 * - layer (TODO)
 * - type: simple or animated
 * - for an animated tile: the coordinates and the sequence type
 */

/**
 * Number of times the screen is redrawn is a second.
 */
const int FRAMES_PER_SECOND = 25;

/**
 * Delay between two frames in millisecond.
 */
const int FRAME_INTERVAL = 1000 / FRAMES_PER_SECOND;

/**
 * Creates a tileset editor.
 * @param tileset_name name of the tileset to edit
 * (if the corresponding file doesn't exist, this will be a new tileset)
 * This name is used to compute the tileset file name, the image file name
 * and the name of the C++ file generated.
 */
TilesetEditor::TilesetEditor(const char *tileset_name) {
  this->nb_tiles = 0;

  if (strlen(tileset_name) > MAX_TILESET_NAME) {
    cerr << "Tileset name too long\n";
    exit(1);
  }

  // tileset file
  sprintf(tileset_file_name, "src/tilesets/%s.zsd", tileset_name);

  // image file
  sprintf(image_file_name, "images/tilesets/%s.png", tileset_name);

  // C++ source file
  sprintf(source_file_name, "src/tilesets/%s.cpp", tileset_name);

  // C++ header file
  sprintf(header_file_name, "include/tilesets/%s.h", tileset_name);
}

/**
 * Destructor.
 */
TilesetEditor::~TilesetEditor(void) {
  for (int i = 0; i < nb_tiles; i++) {
    delete tiles[i];
  }
}

/**
 * Launches the editor and displays it on an SDL surface.
 * @param src_surface surface of the editor
 */
void TilesetEditor::launch(SDL_Surface *src_surface) {
  SDL_Rect selected_area, selected_area_dst, selected_area_border;
  bool is_selecting = false;
  int total_width, total_height;
  int x, y, previous_x2, previous_y2;
  int x2, y2;
  int x_tileset_area = 152;
  int y_tileset_area = 152;
  Uint32 ticks, last_frame_date = 0;  

  // get the total dimensions
  total_width = src_surface->w;
  total_height = src_surface->h;

  // destination area for each component: the tile list, the control panel
  // and the tileset image
  SDL_Rect tile_list_rect = {0, 0, x_tileset_area - 2, total_height};
  SDL_Rect control_panel_rect = {x_tileset_area, 0, total_width - x_tileset_area, 150};
  SDL_Rect tileset_image_rect = {x_tileset_area, y_tileset_area, total_width -  x_tileset_area, total_height - y_tileset_area};
  
  SDL_Surface *tileset_surface = IMG_Load(image_file_name);

  if (tileset_surface == NULL) {
    cerr << "Unable to load tileset " << image_file_name << endl;
    return;
  }

  // main loop
  SDL_Event event;
  bool cont = true;

  while (cont) {
    SDL_WaitEvent(&event);

    switch (event.type) {

    case SDL_QUIT:
      cont = false;
      break;

      // select an area of the tileset
    case SDL_MOUSEBUTTONDOWN:

      if (event.button.button == SDL_BUTTON_LEFT && !is_selecting) {

	if (event.button.x >= tileset_image_rect.x && event.button.y >= tileset_image_rect.y
	    && event.button.x < tileset_image_rect.x + tileset_image_rect.w
	    && event.button.y < tileset_image_rect.y + tileset_image_rect.h) {
      
	  x = event.button.x + 4;
	  y = event.button.y + 4;
	
	  x = x - (x % 8);
	  y = y - (y % 8);

	  previous_x2 = 0;
	  previous_y2 = 0;

	  selected_area_border.x = x;
	  selected_area_border.y = y;
	  selected_area_border.w = 0;
	  selected_area_border.h = 0;

	  is_selecting = true;
	}
      }
      break;

    case SDL_MOUSEBUTTONUP:
      is_selecting = false;
      break;

    case SDL_MOUSEMOTION:
      if (is_selecting) {

	x2 = event.motion.x + 4;
	y2 = event.motion.y + 4;

	x2 = x2 - (x2 % 8);
	y2 = y2 - (y2 % 8);

	if (x2 < tileset_image_rect.x) {
	  x2 = tileset_image_rect.x;
	}
	else if (x2 > tileset_image_rect.x + tileset_image_rect.w) {
	  x2 = tileset_image_rect.x + tileset_image_rect.w;
	}

	if (y2 < tileset_image_rect.y) {
	  y2 = tileset_image_rect.y;
	}
	else if (y2 > tileset_image_rect.y + tileset_image_rect.h) {
	  y2 = tileset_image_rect.y + tileset_image_rect.h;
	}

	// change the selected area if necessary
	if (x2 != previous_x2 || y2 != previous_y2) {

	  if (x < x2) {
	    selected_area_border.x = x;
	    selected_area_border.w = x2 - x;
	  }
	  else {
	    selected_area_border.x = x2;
	    selected_area_border.w = x - x2;
	  }
	  
	  if (y < y2) {
	    selected_area_border.y = y;
	    selected_area_border.h = y2 - y;
	  }
	  else {
	    selected_area_border.y = y2;
	    selected_area_border.h = y - y2;
	  }

	  selected_area.x = selected_area_border.x - x_tileset_area + 1;
	  selected_area.y = selected_area_border.y - y_tileset_area + 1;
	  selected_area.w = max(selected_area_border.w - 2, 0);
	  selected_area.h = max(selected_area_border.h - 2, 0);
	  
	  selected_area_dst.x = selected_area_border.x + 1;
	  selected_area_dst.y = selected_area_border.y + 1;
	  
	  previous_x2 = x2;
	  previous_y2 = y2;
	}
      }
      break;

    }

    // display everything each time frame
    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + FRAME_INTERVAL) {
      last_frame_date = ticks;
      
      // background color
      SDL_FillRect(src_surface, NULL, COLOR_BLACK);
      
      // tile list
      SDL_FillRect(src_surface, &tile_list_rect, get_color(255, 255, 0));
      
      // control panel
      display_control_panel(src_surface, control_panel_rect);
      
      // tileset image
      SDL_BlitSurface(tileset_surface, NULL, src_surface, &tileset_image_rect);
      if (is_selecting && selected_area.w > 0 && selected_area.h > 0) {
	SDL_FillRect(src_surface, &selected_area_border, get_color(0, 255, 0));

	SDL_BlitSurface(tileset_surface, &selected_area, src_surface, &selected_area_dst);
      }
      
      SDL_Flip(src_surface);
    }
  }

  // free everything
  SDL_FreeSurface(tileset_surface);
  SDL_FreeSurface(src_surface);
}

/**
 * Displays the control panel.
 * @param dst the surface
 * @param dst_rect position of the control panel on this surface
 */
void TilesetEditor::display_control_panel(SDL_Surface *dst, SDL_Rect &dst_rect) {
  SDL_FillRect(dst, &dst_rect, get_color(128, 128, 255));
}

/**
 * Saves the tileset file.
 */
void save(void) {
  // TODO
}
  
/**
 * Generates the C++ files.
 */
void generate_code(void) {
  // TODO
}

/**
 * Main function.
 * Usage: ./bin/tileset_editor tileset_name
 */
int main(int argc, char **argv) {

  // check the parameter
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " tileset_name\n";
    exit(1);
  }

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_Surface *screen = SDL_SetVideoMode(1000, 750, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

  color_init();
  
  TilesetEditor *editor = new TilesetEditor(argv[1]);
  editor->launch(screen);

  delete editor;
  SDL_Quit();
}
