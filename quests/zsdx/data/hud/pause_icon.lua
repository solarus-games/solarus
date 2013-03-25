-- The icon that shows what the pause command does.

local pause_icon = {}

function pause_icon:new(game)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game)

  return object
end

function pause_icon:initialize(game)

  self.game = game
  self.is_game_paused = false
  self.surface = sol.surface.create(72, 24)
  self.surface:set_transparency_color{0, 0, 0}
  self.icons_img = sol.surface.create("pause_icon.png", true)
  self.icon_region_y = 24

  local pause_icon = self
  self.icon_flip_sprite = sol.sprite.create("hud/pause_icon_flip")

  function self.icon_flip_sprite:on_animation_finished()
    if pause_icon.icon_region_y == nil then
      pause_icon.icon_region_y = 24
      if pause_icon.game:is_paused() then
        pause_icon.icon_region_y = 48
      end
      pause_icon:rebuild_surface()
    end
  end

  function self.icon_flip_sprite:on_frame_changed()
    pause_icon:rebuild_surface()
  end

  self:rebuild_surface()
end

function pause_icon:on_paused()
  self.icon_region_y = nil
  self.icon_flip_sprite:set_frame(0)
  self:rebuild_surface()
end

function pause_icon:on_unpaused()
  self.icon_region_y = nil
  self.icon_flip_sprite:set_frame(0)
  self:rebuild_surface()
end

function pause_icon:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  if self.icon_region_y ~= nil then
    -- Draw the static image of the icon: "Pause" or "Back".
    self.icons_img:draw_region(0, self.icon_region_y, 72, 24, self.surface)
  else
    -- Draw the flipping sprite
    self.icon_flip_sprite:draw(self.surface, 24, 0)
  end
end

function pause_icon:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function pause_icon:on_draw(dst_surface)

  local map = self.game:get_map()
  if map == nil or not map:is_dialog_enabled() then
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

return pause_icon

