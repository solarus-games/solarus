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

