/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_TITLE_SCREEN_H
#define ZSDX_TITLE_SCREEN_H

#include "Common.h"
#include "Screen.h"
#include "lowlevel/Rectangle.h"

/**
 * This class shows the title screen of the game.
 */
class TitleScreen: public Screen {

  private:

    /**
     * The phases of the title screen.
     */
    enum Phase {
      PHASE_BLACK_SCREEN,
      PHASE_ZS_PRESENTS,
      PHASE_TITLE
    };

    /**
     * Possible times of the day, used
     * to display different title screens.
     */
    enum TimeOfDay {
      DAYLIGHT,
      SUNSET,
      NIGHT
    };

    Phase current_phase;
    uint32_t next_phase_date;
    TransitionFade *transition_in;
    TransitionFade *transition_out;

    // phase 1
    void init_phase_black_screen(void);

    // phase 2
    Surface *zs_presents_img;
    Rectangle zs_presents_position;

    void init_phase_zs_presents(void);
    void exit_phase_zs_presents(void);

    // phase 3
    Music *title_screen_music;
    Surface *background_img;
    Surface *clouds_img;
    Surface *logo_img;
    Surface *dx_img;
    Surface *star_img;
    TextSurface *website_img;
    TextSurface *press_space_img;
    Surface *title_surface;
    int counter;
    uint32_t next_image_date;
    Rectangle clouds_position;
    uint32_t next_clouds_move_date;

    void init_phase_title(void);
    void exit_phase_title(void);
    void update_phase_title(void);
    void display_phase_title(Surface *destination_surface);

    TimeOfDay get_time_of_day(void);

  public:

    // creation and destruction
    TitleScreen(Solarus *solarus);
    ~TitleScreen(void);

    // update and display
    void handle_event(const SDL_Event &event);
    void update(void);
    void display(Surface *destination_surface);
};

#endif

