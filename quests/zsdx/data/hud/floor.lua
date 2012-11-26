-- The floor view shown when entering a map that has a floor.

local floor_view = {}

function floor_view:new(game)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game)

  return object
end

function floor_view:initialize(game)

  self.game = game
  self.surface = sol.surface.create(32, 85)
  self.surface:set_transparency_color{0, 0, 0}
  self.floors_img = sol.surface.create("floors.png", true)  -- Language-specific image

  self:check()
  self:rebuild_surface()
end

function floor_view:check()

  -- TOOD
end

function floor_view:on_map_changed(map)

  -- TODO
end

function floor_view:rebuild_surface()

  self.surface:fill_color{0, 0, 0}
  -- TODO
end

function floor_view:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function floor_view:on_draw(dst_surface)

  local x, y = self.dst_x, self.dst_y
  local width, height = dst_surface:get_size()
  if x < 0 then
    x = width + x
  end
  if y < 0 then
    y = height + y
  end

  self.surface:draw(dst_surface, x, y)
end

return floor_view

