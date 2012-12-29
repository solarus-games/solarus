local submenu = require("menus/pause_submenu")
local map_submenu = submenu:new()

function map_submenu:on_started()
  submenu.on_started(self)
end

function map_submenu:on_command_pressed(command)

  local handled = submenu.on_command_pressed(self, command)

  if not handled then
    if command == "left" then
      self:previous_submenu()
      handled = true
    elseif command == "right" then
      self:next_submenu()
      handled = true
    end
  end

  return handled
end

function map_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_save_dialog_if_any(dst_surface)
end

return map_submenu

