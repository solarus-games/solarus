-- Lua script of enemy slime_red_egg.
local enemy = ...
local map = enemy:get_map()

local time_to_hatch = 5000
local can_procreate = true -- Allow the new slime to create more eggs.
local slime_model = "slime_red" -- Type of the new slime.

-- Event called when the enemy is initialized.
function enemy:on_created()
  local sprite = enemy:create_sprite("enemies/" .. enemy:get_breed())
  sprite:set_animation("egg_hatching")
  self:set_pushed_back_when_hurt(false)
  enemy:set_life(1)
  enemy:set_damage(1)
end

-- Event called when the enemy is restarted.
function enemy:on_restarted()
  -- Remove the egg after a delay and create a red slime.
  sol.timer.start(self, time_to_hatch, function()
    self:get_sprite():set_animation("egg_explode")
    sol.audio.play_sound("explosion")
    sol.timer.start(self, 200, function()
      local x, y, layer = self:get_position()
      local prop = {x = x, y = y, layer = layer, direction = 0, breed = slime_model}
      local slime = map:create_enemy(prop)
      slime:set_egg_enabled(false) -- Do not allow to procreate again.
      slime:jump() -- Start jump when created.
      self:remove() 
    end)
  end)
end

-- Model for the slime to be born.
function enemy:set_slime_model(enemy_id) slime_model = enemy_id end

-- Falling animation from certain height.
function enemy:fall(initial_height)
  local height = initial_height or 0
  sol.audio.play_sound("throw")
  local fall_duration = 1000 -- Duration of the first part of the fall.
  local max_height = 24
  local sprite = self:get_sprite()
  sprite:set_animation("egg")
  -- Create shadow.
  local shadow = self:create_sprite("shadows/shadow_small")
  -- Shift sprite of egg.
  local function f(t) -- Shifting function.
    return math.floor(4 * max_height * (t / fall_duration - (t / fall_duration) ^ 2) + height)
  end
  local t = 0
  local refreshing_time = 10
  sol.timer.start(enemy, refreshing_time, function() -- Update shift each 10 milliseconds.
    local shift = - f(t)
    sprite:set_xy(0, shift)
    t = t + refreshing_time
    if shift >= 0 and t >= fall_duration then
      sprite:set_xy(0, 0)
      sol.audio.play_sound("bomb") -- Sound when fall on ground.
      self:remove_sprite(shadow)
      sprite:set_animation("egg_hatching")
      return false
    else return true end
  end)
end

-- Determine if the new red slime can procreate or not.
function enemy:set_can_procreate(bool) can_procreate = bool end
