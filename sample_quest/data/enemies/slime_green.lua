local enemy = ...

local life = 1
local damage = 1
local state -- States: "stopped", "going_hero", "hidden", "hiding", "unhiding", "jumping", "prepare_jump", "finish_jump".
local speed = 20
local detection_distance = 80
local jump_duration = 1000 -- Time in milliseconds.
local max_height = 24 -- Height for the jump, in pixels.
local jumping_speed = 60 -- Speed of the movement during the jump.
local is_pushing_enemy = false -- Used to avoid a chain reaction of too many collisions (that makes a crash).

function enemy:on_created()
  self:set_life(life)
  self:set_damage(damage)
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
  self:set_obstacle_behavior("flying") -- Allow to traverse bad grounds (and fall on them).
  local sprite = self:get_sprite()
  if not sprite then sprite = self:create_sprite("enemies/slime_green") end
  state = "hidden"
  function sprite:on_animation_finished(animation)
    if animation == "hide" then
      state = "hidden"
      sprite:set_animation("hidden")
      enemy:restart() -- Restart enemy after hiding.
    elseif animation == "unhide" then
      enemy:start_going_hero()
    elseif animation == "prepare_jump" then
      enemy:jump()
    elseif animation == "finish_jump" then
      state = "stopped"
      sprite:set_animation("stopped")
      sol.timer.start(self, 200, function()
        enemy:start_going_hero()
      end)
    end
  end
end

function enemy:on_restarted()
  -- Allow to push enemies.
  is_pushing_enemy = false
  -- Reset the starting animation if necessary (the engine sets the "walking" animation).
  if state == "hidden" then
    self:get_sprite():set_animation("hidden")
  end
  -- Check for bad grounds.
  self:start_checking_ground()
  -- Start looking for hero.
  self:start_checking()
end

-- Start checking for hero.
function enemy:start_checking()
  local hero = self:get_map():get_hero()
  -- Start loop for checking.
  sol.timer.start(self, 30, function()
    --print(state)
    local is_close = (self:get_distance(hero) <= detection_distance)
    if is_close then
      -- Unhide and follow hero if hero is close.
      if state == "hidden" then
        self:unhide()
      end
    elseif (not is_close) then
      -- Hide and stop if hero is not close.
      if state == "going_hero" then
        self:hide()
      end
    end
    return true
  end)
end

-- Go to hero.
function enemy:start_going_hero()
  state = "going_hero"
  self:get_sprite():set_animation("walking")
  local m = sol.movement.create("target")
  m:set_speed(speed)
  m:set_target(self:get_map():get_hero())
  m:start(self)
  -- Prepare jump.
  sol.timer.start(self, 1500, function()
    self:prepare_jump()
  end)
end

-- Hide.
function enemy:hide()
  state = "hiding"
  self:stop_movement()
  self:get_sprite():set_animation("hide")
end
-- Unhide.
function enemy:unhide()
  state = "unhiding"
  self:get_sprite():set_animation("unhide")
end
-- Prepare jump.
function enemy:prepare_jump()
  state = "prepare_jump"
  self:stop_movement()
  --sol.timer.start(self, 1000, function()
    self:get_sprite():set_animation("prepare_jump")
  --end)
end
-- Finish jump.
function enemy:finish_jump()
  state = "finish_jump"
  self:stop_movement()
  self:get_sprite():set_animation("finish_jump")
end

-- Jump.
function enemy:jump()
  -- Set jumping state, animation and sound.
  state = "jumping"
  local sprite = self:get_sprite()
  sprite:set_animation("jump")
  sol.audio.play_sound("jump")
  self:set_invincible(true) -- Set invincible.
  -- Start shift on sprite.
  local function f(t) -- Shifting function.
    return math.floor(4 * max_height * (t / jump_duration - (t / jump_duration) ^ 2))
  end
  local t = 0
  local refreshing_time = 10
  sol.timer.start(self, refreshing_time, function() -- Update shift each 10 milliseconds.
    sprite:set_xy(0, -f(t))
    t = t + refreshing_time
    if t > jump_duration then return false
      else return true
    end
  end)
  -- Add a shadow sprite.
  local shadow = self:create_sprite("things/ground_effects")
  shadow:set_animation("shadow_big_dynamic")
  local new_frame_delay = math.floor(jump_duration/shadow:get_num_frames())
  shadow:set_frame_delay(new_frame_delay)
  -- Add movement towards near the hero during the jump. The jump does not target the hero.
  -- The angle is partially random to avoid too many enemies overlapping.
  local m = sol.movement.create("straight")
  local angle = self:get_angle(self:get_map():get_hero())
  math.randomseed(os.time()) -- Initialize random seed.
  local d = 2*math.random() - 1 -- Random real number in [-1,1].
  angle = angle + d*math.pi/4 -- Alter jumping angle, randomly.
  m:set_speed(jumping_speed)
  m:set_angle(angle)
  m:start(self)
  -- Finish the jump.
  sol.timer.start(self, jump_duration, function()
    self:remove_sprite(shadow) -- Remove shadow sprite.
    self:set_default_attack_consequences() -- Stop invincibility after jump.
    self:finish_jump()
  end)
end

-- Add an "splash" sprite when dying.
function enemy:on_dying()
  local splash = self:create_sprite("enemies/slime_green")
  splash:set_animation("pieces")
  splash:set_xy(0,-14)
end

-- Check for bad ground (water, hole and lava) and also for empty ground.
function enemy:check_on_ground()
  local map = self:get_map()
  local x, y, layer = self:get_position()
  local ground = map:get_ground(x, y, layer)
  if ground == "empty" and layer > 0 then
    -- Fall to lower layer and check ground again.
     self:set_position(x, y, layer-1)
     self:check_on_ground() -- Check again new ground.
  elseif ground == "hole" then
    -- Create falling animation centered correctly on the 8x8 grid.
    x = math.floor(x/8)*8 + 4; if map:get_ground(x, y, layer) ~= "hole" then x = x + 4 end
    y = math.floor(y/8)*8 + 4; if map:get_ground(x, y, layer) ~= "hole" then y = y + 4 end
    local fall_on_hole = map:create_custom_entity({x = x, y = y, layer = layer, direction = 0})
    local sprite = fall_on_hole:create_sprite("things/ground_effects")
    sprite:set_animation("hole_fall")
    self:remove()
    function sprite:on_animation_finished() fall_on_hole:remove() end
    sol.audio.play_sound("falling_on_hole")
  elseif ground == "deep_water" then
    -- Sink in water.
    local water_splash = map:create_custom_entity({x = x, y = y, layer = layer, direction = 0})
    local sprite = water_splash:create_sprite("things/ground_effects")
    sprite:set_animation("water_splash")
    self:remove()
    function sprite:on_animation_finished() water_splash:remove() end
    sol.audio.play_sound("splash")
  elseif ground == "lava" then
    -- Sink in lava.
    local lava_splash = map:create_custom_entity({x = x, y = y, layer = layer, direction = 0})
    local sprite = lava_splash:create_sprite("things/ground_effects")
    sprite:set_animation("lava_splash")
    self:remove()
    function sprite:on_animation_finished() lava_splash:remove() end
    sol.audio.play_sound("splash")
  end
end

-- Start a timer to check ground once per second (useful if the ground moves or changes type!!!).
function enemy:start_checking_ground()
  sol.timer.start(self, 300, function()
    if state == "jumping" then return true end -- Do not check the ground while jumping.
    self:check_on_ground()
    return true
  end)
end
