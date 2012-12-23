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
  self.opacity = 255
  self.icons_img = sol.surface.create("sword_icon.png", true)
  self.current_icon_img = sol.surface.create(self.icons_img, 0, 24, 72, 24)

  local attack_icon = self
  self.icon_flip_sprite = sol.sprite.create("hud/sword_icon_flip")

  function self.icon_flip_sprite:on_animation_finished()
    if attack_icon.current_icon_img == nil then
      local y = 0 -- TODO look for attack command effect and the current sword.
      attack_icon.current_icon_img = sol.surface.create(attack_icon.icons_img, 0, y, 72, 24)
      attack_icon:rebuild_surface()
    end
  end

  function self.icon_flip_sprite:on_frame_changed()
    attack_icon:rebuild_surface()
  end

  self:rebuild_surface()
end

function attack_icon:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  if self.current_icon_img ~= nil then
    -- Draw the static image of the icon.
    self.current_icon_img:draw(self.surface)
  else
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

