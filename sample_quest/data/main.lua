-- This is the main Lua script of your project.
-- You will probably make a title screen and then start a game.
-- See the Lua API! http://www.solarus-games.org/solarus/documentation/

function sol.main:on_started()
  -- This function is called when Solarus starts.
  print("This is a sample quest for Solarus.")

  -- Setting a language is useful to display text and dialogs.
  sol.language.set_language("en")

  -- Show the Solarus logo initially.
  local solarus_logo_menu = require("menus/solarus_logo")
  sol.menu.start(self, solarus_logo_menu)
end

