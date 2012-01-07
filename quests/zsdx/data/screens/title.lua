-- This screen is displayed when the program starts, after the language
-- selection screen (if any).

local surface

function event_menu_started()

  -- black screen during 0.3 seconds
  surface = sol.surface.create()
  surface:fill_color(255, 255, 0)
  sol.main.timer_start(phase_zs_presents, 3000)
end

function phase_zs_presents()

  local zs_presents_img =
      sol.surface.create("title_screen_initialization.png", true)
  surface:draw(zs_presents_img)
  sol.main.play_sound("intro")
end

function event_display(destination_surface)

  destination_surface:draw(surface)
end

