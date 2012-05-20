-- Main script of the quest.

-- Event called every tick by the engine!
function sol.main:on_update()

end

-- Event called when Lua should show its initial screen.
function sol.main:on_started()

  -- Just need this here, no need to require globally.
  local title_screen = require("screens/title")
  t = title_screen:new()

  -- Show the title screen initially.
  sol.main.start_screen(t)
end

function sol.main:on_key_pressed(key, mods)

  -- print("A key was pressed: " .. key .. "!")
end

