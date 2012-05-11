-- This screen is displayed when the program starts, after the language
-- selection screen (if any).

local phase -- "black", "zs_presents" or "title"
local title_surface -- surface where everything is drawn

function on_menu_started()

  -- black screen during 0.3 seconds
  phase = "black"

  title_surface = sol.surface.create()
  sol.timer.start(300, phase_zs_presents)

  -- use these 0.3 seconds to preload all sound effects
  sol.audio.preload_sounds()
end

function phase_zs_presents()

  -- "Zelda Solarus presents" displayed for two seconds
  phase = "zs_presents"

  local zs_presents_img =
      sol.surface.create("title_screen_initialization.png", true)

  local width, height = zs_presents_img:get_size()
  local x, y = 160 - width / 2, 120 - height / 2
  title_surface:draw(zs_presents_img, x, y)
  sol.audio.play_sound("intro")

  sol.timer.start(2000, phase_title)
end

function phase_title()

  -- actual title screen
  phase = "title"

  -- start music
  sol.audio.play_music("title_screen")

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
  sol.timer.start(5000, function()
    sol.audio.play_sound("ok")
    dx_img = sol.surface.create("menus/title_dx.png")
  end)

  sol.timer.start(6000, function()
    star_img = sol.surface.create("menus/title_star.png")
  end)

  show_press_space = false
  function switch_press_space()
    show_press_space = not show_press_space
    sol.timer.start(500, switch_press_space)
  end
  sol.timer.start(6500, switch_press_space)

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
    sol.timer.start(50, move_clouds)
  end
  sol.timer.start(50, move_clouds)

  -- show an opening transition
  title_surface:fade_in(30)
end

function on_display(dst_surface)

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

function on_key_pressed(key)

  if key == "escape" then
    -- stop the program
    sol.main.exit()

  elseif key == "space" or key == "return" then
    try_finish_title()
  end
end

function on_joypad_button_pressed(button)

  try_finish_title()
end

-- Ends the title screen (if possible)
-- and starts the savegame selection screen
function try_finish_title()

  if phase == "title"
      and dx_img ~= nil
      and not finished then
    finished = true

    title_surface:fade_out(30, function()
      sol.audio.stop_music()
      sol.main.start_screen("savegames")
    end)
  end
end

