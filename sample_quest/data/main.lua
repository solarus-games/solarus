-- This is the main Lua script of your project.
-- You will probably make a title screen and then start a game.
-- See the Lua API! http://www.solarus-games.org/doc/latest

local game_manager = require("scripts/game_manager")

-- This function is called when Solarus starts.
function sol.main:on_started()

  print("This is a sample quest for Solarus.")

  -- Setting a language is useful to display text and dialogs.
  sol.language.set_language("en")

  -- Set the window title.
  sol.video.set_window_title("Sample quest - Solarus " .. sol.main.get_solarus_version())

  -- Show the Solarus logo initially.
  local solarus_logo = require("scripts/menus/solarus_logo")
  sol.menu.start(self, solarus_logo)

  -- Start the game when the Solarus logo menu is finished.
  solarus_logo.on_finished = function()
    game_manager:start_game()
  end

end

-- Event called when the player pressed a keyboard key.
function sol.main:on_key_pressed(key, modifiers)

  local handled = false
  if key == "f5" then
    -- F5: change the video mode.
    sol.video.switch_mode()
    handled = true
  elseif key == "f11" or
    (key == "return" and (modifiers.alt or modifiers.control)) then
    -- F11 or Ctrl + return or Alt + Return: switch fullscreen.
    sol.video.set_fullscreen(not sol.video.is_fullscreen())
    handled = true
  elseif key == "f4" and modifiers.alt then
    -- Alt + F4: stop the program.
    sol.main.exit()
    handled = true
  elseif key == "escape" and sol.main.game == nil then
    -- Escape in title screens: stop the program.
    sol.main.exit()
    handled = true
  end

  return handled
end
