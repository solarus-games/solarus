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
  self.icon_region_y = nil
  self.icon_flip_sprite = sol.sprite.create("hud/sword_icon_flip")
  self.is_flipping = false
  self.effect_displayed = self.game.hud.custom_command_effects["attack"] or self.game:get_command_effect("attack")
  self.sword_displayed = game:get_ability("sword")
  self.showing_dialog = false
  self:compute_icon_region_y()

  local attack_icon = self
  function self.icon_flip_sprite:on_animation_finished()
    if attack_icon.is_flipping then
      attack_icon.is_flipping = false
      attack_icon:compute_icon_region_y()
      attack_icon:rebuild_surface()
    end
  end

  function self.icon_flip_sprite:on_frame_changed()
    attack_icon:rebuild_surface()
  end

  self:check()
  self:rebuild_surface()
end

function attack_icon:compute_icon_region_y()

  local map = self.game:get_map()
  if self.effect_displayed ~= nil or map == nil or not map:is_dialog_enabled() then
    if self.effect_displayed == nil then
      -- Show an empty icon.
      self.icon_region_y = 0
    elseif self.effect_displayed == "sword" then
      -- Create an icon with the current sword.
      self.icon_region_y = 96 + 24 * self.sword_displayed
    elseif self.effect_displayed ~= nil then
      -- Create an icon with the name of the current effect.
      local effects_indexes = {
        ["save"] = 1,
        ["return"] = 2,
        ["validate"] = 3,
        ["skip"] = 4,
      }
      self.icon_region_y = 24 * effects_indexes[self.effect_displayed]
    end
  end
end

function attack_icon:check()

  local need_rebuild = false

  if not self.flipping then
    local effect = self.game.hud.custom_command_effects["attack"] or self.game:get_command_effect("attack")
    local sword = self.game:get_ability("sword")
    local map = self.game:get_map()
    local showing_dialog = map ~= nil and map:is_dialog_enabled()
    if effect ~= self.effect_displayed
        or sword ~= self.sword_displayed
        or showing_dialog ~= self.showing_dialog then

      if self.effect_displayed ~= nil then
        if effect == nil and showing_dialog then
          self.icon_flip_sprite:set_animation("disappearing")
        else
          self.icon_flip_sprite:set_animation("flip")
        end
      else
        self.icon_flip_sprite:set_animation("appearing")
      end
      self.effect_displayed = effect
      self.sword_displayed = sword
      self.showing_dialog = showing_dialog
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

function attack_icon:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  if self.icon_region_y ~= nil then
    -- Draw the static image of the icon.
    self.icons_img:draw_region(0, self.icon_region_y, 72, 24, self.surface)
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

