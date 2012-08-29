-- Main script of the quest.

local function start_savegame(file_name)
  local game = sol.game.load(file_name)
  local play_game = sol.main.load_file("play_game")
  play_game(game)
end

local console = sol.main.load_file("console")()
sol.main.console = console

-- Event called when Lua should show its initial screen.
function sol.main:on_started()

  -- Just need this here, no need to require globally.
  local language_menu = require("screens/language")

  -- Show the language menu initially.
  sol.main.start_screen(language_menu:new())
end

-- Event called when the player pressed a keyboard key.
function sol.main:on_key_pressed(key, modifiers)

  -- Debug only.
  if sol.main.is_debug_enabled() then

    if console.enabled then
      console:on_key_pressed(key, modifiers)
    else
      if key == "f1" then
	start_savegame("save1.dat")
      elseif key == "f2" then
	start_savegame("save2.dat")
      elseif key == "f3" then
	start_savegame("save3.dat")
      elseif key == "f12" then
	console:start()
      end
    end
  end
end

function sol.main:on_post_draw(dst_surface)

  if console.enabled then
    console:on_draw(dst_surface)
  end
end


