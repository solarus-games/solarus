-- This menu is displayed when the program starts, before the language screen.
-- It shows the Solarus engine logo.

local solarus_logo_menu = {}

local game_manager = require("scripts/game_manager")

local finished = false
local surface

local function stop_menu()

  -- Stop this menu and start a game.
  sol.menu.stop(solarus_logo_menu)
  game_manager.start_game()
end

function solarus_logo_menu:on_started()

  finished = false
  surface = sol.surface.create("menus/solarus_logo.png")
  surface:fade_in()
  sol.timer.start(solarus_logo_menu, 1000, function()
    if not finished then
      finished = true
      surface:fade_out()
      sol.timer.start(self, 700, stop_menu)
    end
  end)
end

function solarus_logo_menu:on_draw(dst_surface)

  -- The logo makes 320x240 pixels, but dst_surface may be larger.
  local width, height = dst_surface:get_size()
  surface:draw(dst_surface, width / 2 - 160, height / 2 - 120)
end

function solarus_logo_menu:on_key_pressed(key)

  local handled = false

  if key == "escape" then
    -- Stop the program.
    handled = true
    sol.main.exit()

  else

    if not finished then
      handled = true
      finished = true
      stop_menu()
    end

    return handled
  end
end

function solarus_logo_menu:on_joypad_button_pressed(button)

  return solarus_logo_menu:on_key_pressed("space")
end

return solarus_logo_menu

