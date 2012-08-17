-- Main script of the quest.

-- Event called every tick by the engine!
function sol.main:on_update()

end

-- Event called when Lua should show its initial screen.
function sol.main:on_started()

  -- Just need this here, no need to require globally.
  local language_menu = require("screens/language")

  -- Show the language menu initially.
  sol.main.start_screen(language_menu:new())
end

function sol.main:on_key_pressed(key)

  function start_savegame(file_name)
    local game = sol.game.load(file_name)
    local play_game = sol.main.load_file("play_game")
    play_game(game)
  end

  -- Debug only.
  if sol.main.is_debug_enabled() then
    if key == "f1" then
      start_savegame("save1.dat")
    elseif key == "f2" then
      start_savegame("save2.dat")
    elseif key == "f3" then
      start_savegame("save3.dat")
    end
  end
end

