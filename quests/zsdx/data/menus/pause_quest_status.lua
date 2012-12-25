local submenu = require("menus/pause_submenu")
local quest_status_submenu = submenu:new()

function quest_status_submenu:on_command_pressed(command)
  
  if command == "left" then
    self.game:previous_pause_submenu()
  elseif command == "right" then
    self.game:next_pause_submenu()
  end
end

function quest_status_submenu:on_draw(dst_surface)

end

return quest_status_submenu

