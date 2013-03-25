-- The icon that shows what the action command does.

local action_icon = {}

function action_icon:new(game)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game)

  return object
end

function action_icon:initialize(game)

  self.game = game
  self.surface = sol.surface.create(72, 24)
  self.surface:set_transparency_color{0, 0, 0}
  self.icons_img = sol.surface.create("action_icon.png", true)
  self.icon_region_y = nil
  self.icon_flip_sprite = sol.sprite.create("hud/action_icon_flip")
  self.is_flipping = false
  self.effect_displayed = game.hud.custom_command_effects["action"] or game:get_command_effect("action")
  self:compute_icon_region_y()

  local action_icon = self
  function self.icon_flip_sprite:on_animation_finished()
    if action_icon.is_flipping then
      action_icon.is_flipping = false
      action_icon:compute_icon_region_y()
      action_icon:rebuild_surface()
    end
  end

  function self.icon_flip_sprite:on_frame_changed()
    action_icon:rebuild_surface()
  end

  self:check()
  self:rebuild_surface()
end

function action_icon:compute_icon_region_y()

  local y
  if self.effect_displayed ~= nil then
    -- Create an icon with the name of the current effect.
    local effects_indexes = {
      ["validate"] = 1,
      ["next"] = 2,
      ["info"] = 3,
      ["return"] = 4,
      ["look"] = 5,
      ["open"] = 6,
      ["action"] = 7,
      ["lift"] = 8,
      ["throw"] = 9,
      ["grab"] = 10,
      ["stop"] = 11,
      ["speak"] = 12,
      ["change"] = 13,
      ["swim"] = 14,
    }
    self.icon_region_y = 24 * effects_indexes[self.effect_displayed]
  end
end

function action_icon:check()

  local need_rebuild = false

  if not self.flipping then
    local effect = self.game.hud.custom_command_effects["action"] or self.game:get_command_effect("action")
    if effect ~= self.effect_displayed then
      if self.effect_displayed ~= nil then
        if effect ~= nil then
          self.icon_flip_sprite:set_animation("flip")
        else
          self.icon_flip_sprite:set_animation("disappearing")
        end
      else
        self.icon_flip_sprite:set_animation("appearing")
      end
      self.effect_displayed = effect
      self.icon_region_y = nil
      self.is_flipping = true
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

function action_icon:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  if self.icon_region_y ~= nil then
    -- Draw the static image of the icon.
    self.icons_img:draw_region(0, self.icon_region_y, 72, 24, self.surface)
  elseif self.is_flipping then
    -- Draw the flipping sprite
    self.icon_flip_sprite:draw(self.surface, 24, 0)
  end
end

function action_icon:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function action_icon:on_draw(dst_surface)

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

return action_icon

