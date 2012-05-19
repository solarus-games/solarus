-- Main script of the quest.

-- Special function: called every tick by the engine!
function sol.events:on_update()

end

function sol.events:on_started()
  -- Just need this here, no need to require globally.
  local title_screen = require 'screens/title.oop'

  t = title_screen:new()

  -- Call an initial screen. We need to do this.
  sol.main.start_screen(t)
end

function sol.events:on_key_pressed(key, mods)

  -- print("A key was pressed: " .. key .. "!")
end

