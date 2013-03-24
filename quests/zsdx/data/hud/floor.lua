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
  self.visible = false
  self.surface = sol.surface.create(32, 85)
  self.surface:set_transparency_color{0, 0, 0}
  self.floors_img = sol.surface.create("floors.png", true)  -- Language-specific image
  self.floor = nil
end

function floor_view:on_map_changed(map)

  local need_rebuild = false
  local floor = map:get_floor()
  if floor == self.floor
      or (floor == nil and self.game:get_dungeon() == nil) then
    -- No floor or unchanged floor.
    self.visible = false
  else
    -- Show the floor view during 3 seconds.
    self.visible = true
    local timer = sol.timer.start(3000, function()
      self.visible = false
    end)
    timer:set_suspended_with_map(false)
    need_rebuild = true
  end

  self.floor = floor

  if need_rebuild then
    self:rebuild_surface()
  end
end

function floor_view:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  local highest_floor_displayed
  local dungeon = self.game:get_dungeon()

  if dungeon ~= nil and self.floor ~= nil then
    -- We are in a dungeon: show the neighboor floors before the current one.
    local nb_floors = dungeon.highest_floor - dungeon.lowest_floor + 1
    local nb_floors_displayed = math.min(7, nb_floors)

    -- If there are less 7 floors or less, show them all.
    if nb_floors <= 7 then
      highest_floor_displayed = dungeon.highest_floor
    elseif self.floor >= dungeon.highest_floor - 2 then
      -- Otherwise we only display 7 floors including the current one.
      highest_floor_displayed = dungeon.highest_floor
    elseif self.floor <= dungeon.lowest_floor + 2 then
      highest_floor_displayed = dungeon.lowest_floor + 6
    else
      highest_floor_displayed = self.floor + 3
    end

    local src_y = (15 - highest_floor_displayed) * 12
    local src_height = nb_floors_displayed * 12 + 1

    self.floors_img:draw_region(32, src_y, 32, src_height, self.surface)
  else
    highest_floor_displayed = self.floor
  end

  -- Show the current floor then.
  local src_y
  local dst_y

  if self.floor == nil and dungeon ~= nil then
    -- Special case of the unknown floor in a dungeon.
    src_y = 32 * 12
    dst_y = 0
  else
    src_y = (15 - self.floor) * 12
    dst_y = (highest_floor_displayed - self.floor) * 12
  end

  self.floors_img:draw_region(0, src_y, 32, 13, self.surface, 0, dst_y)
end

function floor_view:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function floor_view:on_draw(dst_surface)

  if self.visible then
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
end

return floor_view

