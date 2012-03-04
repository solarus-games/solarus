-- This screen is displayed when the program starts, after the language
-- selection screen (if any).

local phase -- "black", "zs_presents" or "title"
local title_surface -- surface where everything is drawn

function event_menu_started()

  -- black screen during 0.3 seconds
  phase = "black"

  title_surface = sol.surface.create()
  sol.main.timer_start(phase_zs_presents, 300)
end

function phase_zs_presents()

  -- "Zelda Solarus presents" displayed for two seconds
  phase = "zs_presents"

  local zs_presents_img =
      sol.surface.create("title_screen_initialization.png", true)

  local width, height = zs_presents_img:get_size()
  local x, y = 160 - width / 2, 120 - height / 2
  title_surface:draw(zs_presents_img, x, y)
  sol.main.play_sound("intro")

  sol.main.timer_start(phase_title, 2000)
end

function phase_title()

  -- actual title screen
  phase = "title"

  -- start music
  sol.main.play_music("title_screen.spc")

  -- show a background that depends on the hour of the day
  local hours = tonumber(os.date("%H"))
  local time_of_day
  local hour_text_color
  if hours >= 8 and hours < 18 then
    time_of_day = "daylight"
    hour_text_color = {0, 0, 92}
  elseif hours >= 18 and hours < 20 then
    time_of_day = "sunset"
    hour_text_color = {0, 0, 92}
  else
    time_of_day = "night"
    hour_text_color = {255, 128, 0}
  end

  -- create all images
  background_img = sol.surface.create("menus/title_" .. time_of_day
      .. "_background.png")
  clouds_img = sol.surface.create("menus/title_" .. time_of_day
      .. "_clouds.png")
  logo_img = sol.surface.create("menus/title_logo.png")

  website_img = sol.text_surface.create{
    font = "dialog",
    rendering_mode = "blended",
    text_color = hour_text_color,
    text_key = "title_screen.website",
    horizontal_alignment = "center"
  }

  press_space_img = sol.text_surface.create{
    font = "dialog_big",
    rendering_mode = "blended",
    text_color = hour_text_color,
    text_key = "title_screen.press_space",
    horizontal_alignment = "center"
  }

  -- set up the appearance of images and texts
  sol.main.timer_start(function()
    sol.main.play_sound("ok")
    dx_img = sol.surface.create("menus/title_dx.png")
  end, 5000)

  sol.main.timer_start(function()
    star_img = sol.surface.create("menus/title_star.png")
  end, 6000)

  show_press_space = false
  function switch_press_space()
    show_press_space = not show_press_space
    sol.main.timer_start(switch_press_space, 500)
  end
  sol.main.timer_start(switch_press_space, 6500)

  -- make the clouds move
  clouds_xy = {x = 320, y = 240}
  function move_clouds()

    clouds_xy.x = clouds_xy.x + 1
    clouds_xy.y = clouds_xy.y - 1
    if clouds_xy.x >= 535 then
      clouds_xy.x = clouds_xy.x - 535
    end
    if clouds_xy.y < 0 then
      clouds_xy.y = clouds_xy.y + 299
    end
    sol.main.timer_start(move_clouds, 50)
  end
  sol.main.timer_start(move_clouds, 50)

  -- show an opening transition
  title_surface:fade_in(30)
end

function event_display(dst_surface)

  if phase ~= "title" then
    dst_surface:draw(title_surface)
  else
    display_phase_title(dst_surface)
  end
end

function display_phase_title(destination_surface)

  -- background
  title_surface:fill_color({0, 0, 0})
  title_surface:draw(background_img)

  -- clouds
  local x, y = clouds_xy.x, clouds_xy.y
  title_surface:draw(clouds_img, x, y)
  x = clouds_xy.x - 535
  title_surface:draw(clouds_img, x, y)
  x = clouds_xy.x
  y = clouds_xy.y - 299
  title_surface:draw(clouds_img, x, y)
  x = clouds_xy.x - 535
  y = clouds_xy.y - 299
  title_surface:draw(clouds_img, x, y)

  -- website name and logo
  title_surface:draw(website_img, 160, 220)
  title_surface:draw(logo_img)

  if dx_img then
    title_surface:draw(dx_img)
  end
  if star_img then
    title_surface:draw(star_img)
  end
  if show_press_space then
    title_surface:draw(press_space_img, 160, 190)
  end

  -- final blit
  destination_surface:draw(title_surface)
end

function event_key_pressed(key)

  if key == "escape" then
    -- stop the program
    sol.main.exit()

  elseif key == "space" or key == "return" then
    try_finish_title()
  end
end

function event_joypad_button_pressed(button)

  try_finish_title()
end

-- Ends the title screen (if possible)
-- and starts the savegame selection screen
function try_finish_title()

  if phase == "title"
      and dx_img ~= nil
      and not finished then
    finished = true
    sol.main.stop_music()
    sol.main.start_screen("savegames")
  end
end

