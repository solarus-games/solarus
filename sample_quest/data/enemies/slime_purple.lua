local enemy = ...
local map = enemy:get_map()

local life = 5
local damage = 2
local speed = 30
local detection_distance = 100
local attack_distance = 48
local state -- States: "waiting_hero", "start_fly", "end_fly", "going_hero", "attack", "falling", "wind".
local wings, left_wing, right_wing -- Wings entity and sprites.
local left_shift, right_shift = -18, 18 -- Shifts for the wing sprites.
local flying_height = 32 -- Flying height, given in pixels.
local needs_put_egg = true -- true -- Create only one egg per slime.
local wind_duration = 3000 -- Duration of the wind made by its wings.
local wind_distance = 24

function enemy:on_created()
  state = "waiting_hero"
  self:set_life(life)
  self:set_damage(damage)
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
  self:set_obstacle_behavior("flying") -- Allow to traverse bad grounds.
  -- Create body and wing sprites. The wings are sprites of a custom entity.
  self:create_sprite("enemies/slime_purple")
  local x, y, layer = self:get_position()
  local prop = {x = x, y = y, layer = layer, direction = 0, width = 16, height = 16}
  wings = map:create_custom_entity(prop)
  left_wing = wings:create_sprite("things/wings_evil")
  right_wing = wings:create_sprite("things/wings_evil")
  right_wing:synchronize(left_wing)
  local shadow = wings:create_sprite("shadows/shadow_big_dynamic", "shadow") -- Add a shadow sprite.
  sol.timer.start(wings, 1, function()
    if not enemy:exists() then
      wings:remove() -- Remove wings if the enemy has disappeared.
      return
    end
    local x, y, layer = self:get_position()
    wings:set_position(x, y - 1, layer) -- Shift y-coordinate to draw shadow under slime.
    wings:set_drawn_in_y_order(true)
    return true
  end)
  self:set_height(0)
end

function enemy:on_restarted()
  -- Restart animation of wing sprites (the engine sets the "walking" animation by default).
  left_wing:set_animation("evil_wing")
  right_wing:set_animation("evil_wing")
  if state == "waiting_hero" or state == "falling" then
    left_wing:set_direction(2)
    right_wing:set_direction(3)
  else
    left_wing:set_direction(0)
    right_wing:set_direction(1)  
  end
  if state == "falling" then self:fall(); return end -- Do nothing more while falling.
  -- Start checking for hero.
  self:start_checking()
end

function enemy:start_checking()
  local hero = self:get_map():get_hero()
  sol.timer.start(self, 50, function()
    if state == "waiting_hero" then
      -- If waiting for hero, check if he is close. In that case, go to hero.
      if self:get_distance(hero) <= detection_distance then
        -- Start to fly.
        self:start_fly()
      end
    elseif state == "going_hero" then
      local d = self:get_distance(hero)
      if d > detection_distance then
      -- If the hero is too far, stop and wait.
        self:end_fly()
      elseif d <= attack_distance then
        -- Attack the hero if he is close.
        self:attack()
      end
    end
    return true
  end)
end

function enemy:start_fly()
  state = "start_fly"
  self:set_collision_mode_enabled(false) -- Do not allow to attack and be attacked.
  left_wing:set_direction(0)
  right_wing:set_direction(1)
  local sprite = self:get_sprite()
  local shift = 0
  sol.timer.start(self, 50, function()
    if shift == -flying_height then
      self:go_to_hero()
      return false
    end
    shift = shift - 1
    self:set_height(shift)
    return true
  end)
  local shadow = wings:get_sprite("shadow")
  shadow:set_animation("ascend")
  shadow:set_frame_delay(math.floor(1000/8))
end

-- This function is only called when hatching from an egg.
function enemy:jump() enemy:start_fly() end

function enemy:end_fly()
  state = "end_fly"
  self:stop_movement()
  local sprite = self:get_sprite()
  local _,shift = sprite:get_xy()
  sol.timer.start(self, 50, function()
    if shift == 0 then
      left_wing:set_direction(2)
      right_wing:set_direction(3)
      self:set_collision_mode_enabled(true) -- Allow to attack and be attacked.
      state = "waiting_hero"
      return false
    end
    shift = shift + 1
    self:set_height(shift)
    return true
  end)
  local shadow = wings:get_sprite("shadow")
  shadow:set_animation("descend")
  shadow:set_frame_delay(math.floor(1000/8))
end

function enemy:go_to_hero()
  state = "going_hero"
  local m = sol.movement.create("target")
  m:set_speed(speed)
  m:set_target(self:get_map():get_hero())
  m:start(self)
  -- Create an egg if necessary.
  if needs_put_egg then self:create_egg() end
  -- Start wind.
  self:start_wind()
end

function enemy:attack()
  state = "attack"
  local sprite = self:get_sprite()
  local _,shift = sprite:get_xy()
  local shadow = wings:get_sprite("shadow")
  -- Start descending.
  shadow:set_animation("descend")
  shadow:set_frame_delay(math.floor(1000/8))
  sol.timer.start(self, 50, function()
    if shift == -math.floor(flying_height/3) then
      -- Stop descending. Keep current height for a while.
      self:set_collision_mode_enabled(true) -- Allow to attack and be attacked.
      sol.timer.start(self, 2000, function()
        -- Go up again.
        shadow:set_animation("ascend")
        shadow:set_frame_delay(math.floor(1000/8))
        self:set_collision_mode_enabled(false) -- Do not allow to attack and be attacked.
        sol.timer.start(self, 50, function()
          shift = shift - 1
          if shift == -math.floor(flying_height) then
            -- Stop going up. Wait a bit and then go to hero again.
            self:stop_movement()
            sol.timer.start(self, 50, function()
              self:go_to_hero()
              return false
            end)
            return false
          end
          self:set_height(shift)
          return true
        end)
      end)
      return false
    end
    shift = shift + 1
    self:set_height(shift)
    return true
  end)
end

-- Allow to attack the hero and be attacked, or not.
function enemy:set_collision_mode_enabled(enabled)
  -- Set if enemy can attack or not.
  self:set_can_attack(enabled)
  -- Set if the body of the enemy can receive attacks or not.
  local body = self:get_sprite()
  if enabled then -- Stop invincibility.
    self:set_default_attack_consequences()
  else -- Start invincibility.
    self:set_invincible()
  end
end

function enemy:set_height(shift)
  self:get_sprite():set_xy(0, shift)
  if state ~= "falling" then
    -- Make an extra shift of 1 pixel on y-coordinate (used to draw shadow under slime).
    left_wing:set_xy(left_shift, shift - 1)
    right_wing:set_xy(right_shift, shift - 1)
  end
end

function enemy:on_hurt(attack)
  state = "falling"
end

function enemy:on_dying()
  -- Remove wings entity if necessary if the enemy dies with them.
  if wings then wings:remove(); wings = nil end
end

-- Make the enemy fall and lose its wings.
function enemy:fall()
  self:set_invincible()
  -- Make wings disappear and blink.
  local shadow = wings:get_sprite("shadow")
  shadow:set_animation("descend")
  shadow:set_frame_delay(math.floor(1000/8))
  local t = 0
  local blinking_delay = 50
  sol.timer.start(self, blinking_delay, function()
    local lx, ly = left_wing:get_xy()
    left_wing:set_xy(lx - 1, ly - 1)
    local rx, ry = right_wing:get_xy()
    right_wing:set_xy(rx + 1, ry - 1)
    wings:set_visible(not wings:is_visible())
    t = t + blinking_delay
    return true
  end)
  -- Start falling.
  local _,shift = self:get_sprite():get_xy()
  sol.timer.start(self, 50, function()
    if shift == 0 then
      -- Replace by a normal purple slime.
      local x,y,layer = self:get_position()
      local slime = self:get_map():create_enemy({x=x, y=y, layer=layer, direction = 0, breed = "slime_green"})
      local sprite = slime:get_sprite()
      slime:remove_sprite(sprite)
      slime:create_sprite("enemies/slime_purple")
      slime:on_created() -- Restart "on_created" event to redefine events on new sprite.
      slime:set_split_when_hurt(false) -- Do not allow to split.
      self:remove()
      return false
    end
    shift = shift + 1
    self:set_height(shift)
    return true
  end)
end

-- Create egg.
function enemy:create_egg()
  needs_put_egg = false
  local sprite = self:get_sprite()
  sprite:set_animation("jump")
  sol.timer.start(self, 250, function() sprite:set_animation("walking") end)
  local x, y, layer = self:get_position()
  local prop = {x = x, y = y, layer = layer, direction = 0, breed = "slime_egg"}
  local egg = map:create_enemy(prop)
  egg:set_slime_model("slime_purple")
  egg:fall(flying_height) -- Falling animation.
  egg:set_can_procreate(false) -- Do not allow more procreation from the new slime.
  return egg
end

-- Enable/disable putting egg.
function enemy:set_egg_enabled(bool) needs_put_egg = bool end
function enemy:get_egg_enabled() return needs_put_egg end

-- Wind attack.
function enemy:start_wind()
  state = "wind"
  local wind = wings:create_sprite("enemies/slime_purple", "wind")
  wind:set_animation("wind")
  local h = map:get_hero()
  sol.audio.play_sound("wings1")
  -- Push the hero if he is close to the wind.
  local wind_timer = sol.timer.start(self, 30, function()
    if self:get_distance(h) <= wind_distance 
    and h:get_state() ~= "frozen" then
      local d = self:get_direction8_to(h)
      local x, y, layer = h:get_position()
      local dx, dy = 0, 0
      dx = (d == 0 or d == 1 or d == 7) and 1 or dx
      dx = (d == 3 or d == 4 or d == 5) and -1 or dx
      dy = (d == 1 or d == 2 or d == 3) and -1 or dy
      dy = (d == 5 or d == 6 or d == 7) and 1 or dy
      if not h:test_obstacles(dx, dy) then h:set_position(x + dx, y + dy, layer) end
    end
    return true
  end)
  -- Make wings move faster.
  local old_frame_delay = left_wing:get_frame_delay()
  local new_frame_delay = math.floor(old_frame_delay / 4)
  left_wing:set_frame_delay(new_frame_delay)
  right_wing:set_frame_delay(new_frame_delay)
  -- Finish the wind.
  sol.timer.start(self, wind_duration, function()
    state = "going_hero"
    wind_timer:stop()
    wings:remove_sprite(wind)
    left_wing:set_frame_delay(old_frame_delay)
    right_wing:set_frame_delay(old_frame_delay)
    right_wing:synchronize(left_wing)
    self:restart()
  end)
end
