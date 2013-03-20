-- The money counter shown in the game screen.

local rupees = {}

function rupees:new(game)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game)

  return object
end

function rupees:initialize(game)

  self.game = game
  self.surface = sol.surface.create(48, 12)
  self.surface:set_transparency_color{0, 0, 0}
  self.digits_text = sol.text_surface.create{
    font = "white_digits",
    horizontal_alignment = "left",
  }
  self.digits_text:set_text(game:get_money())
  self.rupee_icons_img = sol.surface.create("hud/rupee_icon.png")
  self.rupee_bag_displayed = self.game:get_item("rupee_bag"):get_variant()
  self.money_displayed = self.game:get_money()

  self:check()
  self:rebuild_surface()
end

function rupees:check()

  local need_rebuild = false
  local rupee_bag = self.game:get_item("rupee_bag"):get_variant()
  local money = self.game:get_money()

  -- Max money.
  if rupee_bag ~= self.rupee_bag_displayed then
    need_rebuild = true
    self.rupee_bag_displayed = rupee_bag
  end

  -- Current money.
  if money ~= self.money_displayed then
    need_rebuild = true
    local increment
    if money > self.money_displayed then
      increment = 1
    else
      increment = -1
    end
    self.money_displayed = self.money_displayed + increment

    -- Play a sound if we have just reached the final value.
    if self.money_displayed == money then
      sol.audio.play_sound("rupee_counter_end")

    -- While the counter is scrolling, play a sound every 3 values.
    elseif self.money_displayed % 3 == 0 then
      sol.audio.play_sound("rupee_counter_end")
    end
  end

  -- Redraw the surface only if something has changed.
  if need_rebuild then
    self:rebuild_surface()
  end

  -- Schedule the next check.
  sol.timer.start(self.game, 40, function()
    self:check()
  end)
end

function rupees:rebuild_surface()

  self.surface:fill_color{0, 0, 0}

  -- Max money (icon).
  self.rupee_icons_img:draw_region((self.rupee_bag_displayed - 1) * 12, 0, 12, 12, self.surface)

  -- Current rupee (counter).
  local max_money = self.game:get_max_money()
  if self.money_displayed == max_money then
    self.digits_text:set_font("green_digits")
  else
    self.digits_text:set_font("white_digits")
  end
  self.digits_text:set_text(self.money_displayed)
  self.digits_text:draw(self.surface, 16, 5)
end

function rupees:set_dst_position(x, y)
  self.dst_x = x
  self.dst_y = y
end

function rupees:on_draw(dst_surface)

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

return rupees

