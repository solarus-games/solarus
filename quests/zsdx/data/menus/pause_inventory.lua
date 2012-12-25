local submenu = require("menus/pause_submenu")
local inventory_submenu = submenu:new()

function inventory_submenu:on_command_pressed(command)
  
  if command == "left" then
    self.game:previous_pause_submenu()
  elseif command == "right" then
    self.game:next_pause_submenu()
  end
end

function inventory_submenu:on_draw(dst_surface)

end

return inventory_submenu

