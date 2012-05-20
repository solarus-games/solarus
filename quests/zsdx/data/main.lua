-- Main script of the quest.

-- Event called every tick by the engine!
function sol.main:on_update()

end

-- Event called when Lua should show its initial screen.
function sol.main:on_started()

  -- Just need this here, no need to require globally.
  local title_screen = require("screens/title")
  local t = title_screen:new()

  -- Show the title screen initially.
  sol.main.start_screen(t)
end

function sol.main:on_key_pressed(key, mods)

  -- TODO remove these temporary keys
  -- print("A key was pressed: " .. key .. "!")
  if key == "pause" then
    local savegame_menu = require("screens/savegames")
    sol.main.start_screen(savegame_menu:new())
  end
end

