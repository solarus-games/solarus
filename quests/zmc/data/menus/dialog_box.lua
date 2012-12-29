local game = ...

local dialog_box = {}

function dialog_box:new(game)

  local object = { game = game }
  setmetatable(object, self)
  self.__index = self

  return object
end

function dialog_box:on_draw(dst_surface)

  -- The dialog box is actually still built-in for now.
  local map = self.game:get_map()
  if map ~= nil and map:is_dialog_enabled() then
    map:draw_dialog_box(dst_surface)
  end
end

function game:initialize_dialog_box()

  self.dialog_box = dialog_box:new(self)
  sol.menu.start(self, self.dialog_box)
end

