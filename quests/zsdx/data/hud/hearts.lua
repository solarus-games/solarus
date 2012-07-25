-- Hearts view used in game screen and in the savegames selection screen.

local hearts = {}

function hearts:new(game)

  local object = {}
  setmetatable(object, self)
  self.__index = self

  object:initialize(game)

  return object
end

function hearts:initialize(game)

  self.game = game
  self.surface = sol.surface.create(90, 18)
  self.surface:set_transparency_color{0, 0, 0}
  self.empty_heart_sprite = sol.sprite.create("hud/empty_heart")
  self.nb_max_hearts_displayed = game:get_max_life() / 4
  self.nb_current_hearts_displayed = game:get_life()

  local all_hearts_img = sol.surface.create("hud/hearts.png")
  self.heart_imgs = {  -- Fractions of hearts.
    sol.surface.create(all_hearts_img,  0, 0, 9, 9),  -- 1/4
    sol.surface.create(all_hearts_img,  9, 0, 9, 9),  -- 2/4
    sol.surface.create(all_hearts_img, 18, 0, 9, 9),  -- 3/4
    sol.surface.create(all_hearts_img, 27, 0, 9, 9)  -- Full heart.
  }
  for i = 1, 4 do
    self.heart_imgs[i]:set_transparency_color{0, 0, 0}
  end

  self:check()

  self:rebuild_surface()
end

-- Checks whether the view displays the correct info
-- and updates it if necessary.
function hearts:check()

  local need_rebuild = false

  -- Maximum life.
  local nb_max_hearts = self.game:get_max_life() / 4
  if nb_max_hearts ~= self.nb_max_hearts_displayed then
    need_rebuild = true
    self.nb_max_hearts_displayed = nb_max_hearts
  end

  -- Current life.
  local nb_current_hearts = self.game:get_life()
  if nb_current_hearts ~= self.nb_current_hearts_displayed then

    need_rebuild = true
    if nb_current_hearts < self.nb_current_hearts_displayed then
      self.nb_current_hearts_displayed = self.nb_current_hearts_displayed - 1
    else
      self.nb_current_hearts_displayed = self.nb_current_hearts_displayed + 1
      if self.game:is_started()
	  and self.nb_current_hearts_displayed % 4 == 0 then
	sol.audio.play_sound("heart")
      end
    end
  end

  -- If we are in-game, play an animation and a sound if the life is low.
  if self.game:is_started() then

    need_rebuild = true
    if self.game:get_life() <= self.game:get_max_life() / 4
        and not self.game:is_suspended() then

      if self.empty_heart_sprite:get_animation() ~= "danger" then
	self.empty_heart_sprite:set_animation("danger")
	sol.timer.start(self.game, 250, function()
	  self:repeat_danger_sound()
	end)
      end
    elseif self.empty_heart_sprite:get_animation() ~= "normal" then
      self.empty_heart_sprite:set_animation("normal")
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

function hearts:repeat_danger_sound()

  if self.game:get_life() <= self.game:get_max_life() / 4
      and not self.game:is_suspended() then

    sol.audio.play_sound("danger")
    sol.timer.start(self.game, 750, function()
      self:repeat_danger_sound()
    end)
  end
end

function hearts:rebuild_surface()

  -- Transparency color.
  self.surface:fill_color{0, 0, 0}

  -- Display the hearts.
  for i = 0, self.nb_max_hearts_displayed - 1 do
    local x, y = (i % 10) * 9, math.floor(i / 10) * 9
    self.empty_heart_sprite:draw(self.surface, x, y)
    if i < math.floor(self.nb_current_hearts_displayed / 4) then
      -- This heart is full.
      self.heart_imgs[4]:draw(self.surface, x, y)
    end
  end

  -- Last fraction of heart.
  local i = math.floor(self.nb_current_hearts_displayed / 4)
  local remaining_fraction = self.nb_current_hearts_displayed % 4
  if remaining_fraction ~= 0 then
    local x, y = (i % 10) * 9, math.floor(i / 10) * 9
    self.heart_imgs[remaining_fraction]:draw(self.surface, x, y)
  end
end

function hearts:display(dst_surface, x, y)

  -- Everything was already drawn on self.surface.
  self.surface:draw(dst_surface, x, y)
end

return hearts

