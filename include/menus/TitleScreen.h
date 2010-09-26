/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_TITLE_SCREEN_H
#define SOLARUS_TITLE_SCREEN_H

#include "Common.h"
#include "Screen.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Shows the title screen of the game.
 *
 * This screen is the first one displayed when the program starts.
 */
class TitleScreen: public Screen {

  private:

    /**
     * @brief The phases of the title screen.
     */
    enum Phase {
      PHASE_BLACK_SCREEN,
      PHASE_ZS_PRESENTS,
      PHASE_TITLE
    };

    /**
     * @brief Possible times of the day.
     *
     * These values are used to display different title screens.
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
    void init_phase_black_screen();

    // phase 2
    Surface *introduction_message_img;
    Rectangle introduction_message_position;

    void init_phase_introduction_message();
    void exit_phase_introduction_message();

    // phase 3
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

    void init_phase_title();
    void exit_phase_title();
    void update_phase_title();
    void display_phase_title(Surface *destination_surface);

    TimeOfDay get_time_of_day();

  public:

    // creation and destruction
    TitleScreen(Solarus *solarus);
    ~TitleScreen();

    // update and display
    void notify_event(InputEvent &event);
    void update();
    void display(Surface *destination_surface);
};

#endif


