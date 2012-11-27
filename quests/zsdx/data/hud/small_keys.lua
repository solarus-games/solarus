-- The small keys counter shown during dungeons or maps with small keys enabled.

local small_keys = {}

function small_keys:new(game)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game)

  return object
end

function small_keys:initialize(game)

  self.game = game
  self.visible = false
  self.surface = sol.surface.create(40, 8)
  self.surface:set_transparency_color{0, 0, 0}
  self.icon_img = sol.surface.create("hud/small_key_icon.png")
  self.digits_text = sol.text_surface.create{
    font = "white_digits",
    horizontal_alignment = "left",
    vertical_alignment = "top",
  }

  self:check()
  self:rebuild_surface()
end

function small_keys:check()

  local need_rebuild = false

  -- Check the number of small keys.
  if self.game:are_small_keys_enabled() then
    local nb_keys = self.game:get_num_small_keys()
    local nb_keys_displayed = tonumber(self.digits_text:get_text())
    if nb_keys_displayed ~= nb_keys then
      self.digits_text:set_text(nb_keys)
      need_rebuild = true
    end
  end

  local visible = self.game:are_small_keys_enabled()
  if visible ~= self.visible then
    self.visible = visible
    need_rebuild = true
  end

  -- Redraw the surface is something has changed.
  if need_rebuild then
    self:rebuild_surface()
  end

  -- Schedule the next check.
  sol.timer.start(self.game, 40, function()
    self:check()
  end)
end

function small_keys:rebuild_surface()

  self.surface:fill_color{0, 0, 0}
  self.icon_img:draw(self.surface)
  self.digits_text:draw(self.surface, 10, 0)
end

function small_keys:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function small_keys:on_draw(dst_surface)

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

return small_keys

