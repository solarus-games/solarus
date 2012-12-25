local submenu = require("menus/pause_submenu")
local options_submenu = submenu:new()

function options_submenu:on_draw(dst_surface)

end

function options_submenu:on_command_pressed(command)
  
  if command == "left" then
    self.game:previous_pause_submenu()
  elseif command == "right" then
    self.game:next_pause_submenu()
  end
end

return options_submenu

