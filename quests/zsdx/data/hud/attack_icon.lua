-- The icon that shows what the attack command does.

local attack_icon = {}

function attack_icon:new(game)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game)

  return object
end

function attack_icon:initialize(game)

  self.game = game
  self.surface = sol.surface.create(72, 24)
  self.surface:set_transparency_color{0, 0, 0}
  self.icons_img = sol.surface.create("sword_icon.png", true)
  self.current_icon_img = sol.surface.create(self.icons_img, 0, 24, 72, 24)
  self.icon_flip_sprite = sol.sprite.create("hud/sword_icon_flip")
  self.is_flipping = false
  self.effect_displayed = game:get_command_effect("attack")
  self.sword_displayed = game:get_ability("sword")
  self:create_icon_img()

  local attack_icon = self
  function self.icon_flip_sprite:on_animation_finished()
    if attack_icon.is_flipping then
      attack_icon.is_flipping = false
      attack_icon:create_icon_img()
      attack_icon:rebuild_surface()
    end
  end

  function self.icon_flip_sprite:on_frame_changed()
    attack_icon:rebuild_surface()
  end

  self:check()
  self:rebuild_surface()
end

function attack_icon:create_icon_img()

  local y
  if self.effect_displayed == nil then
    y = 0
  elseif self.effect_displayed == "sword" then
    -- Create an icon with the current sword.
    y = 96 + 24 * self.sword_displayed
  else
    -- Create an icon with the name of the current effect.
    local effects_y = {
      ["save"] = 24,
      ["return"] = 48,
      ["validate"] = 72,
      ["skip"] = 96,
    }
    y = effects_y[self.effect_displayed]
  end

  self.current_icon_img = sol.surface.create(self.icons_img, 0, y, 72, 24)
end

function attack_icon:check()

  local need_rebuild = false

  if not self.flipping then
    local effect = self.game:get_command_effect("attack")
    local sword = self.game:get_ability("sword")
    if effect ~= self.effect_displayed
        or sword ~= self.sword_displayed then
      self.effect_displayed = effect
      self.sword_displayed = sword
      self.current_icon_img = nil
      self.is_flipping = true
      self.icon_flip_sprite:set_frame(0)
      need_rebuild = true
    end
  end

  -- Redraw the surface only if something has changed.
  if need_rebuild then
    self:rebuild_surface()
  end

  -- Schedule the next check.
  sol.timer.start(self.game, 50, function()
    self:check()
  end)
end

function attack_icon:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  if self.current_icon_img ~= nil then
    -- Draw the static image of the icon.
    self.current_icon_img:draw(self.surface)
  elseif self.is_flipping then
    -- Draw the flipping sprite
    self.icon_flip_sprite:draw(self.surface, 24, 0)
  end
end

function attack_icon:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function attack_icon:on_draw(dst_surface)

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

return attack_icon

