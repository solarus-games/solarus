-- Blacksmith script (enemy).
local enemy = ...

local state = "walk" -- Possible values: "walk", "jump", "stomp".
local phase = 1 -- Phases: 1- Run to hero (15 hits). 2- Jumping (10 hits). 3- Hammer attack (5 hits).
local speed = 10 -- Initial speed. This increases when hurt at first phase.
local current_jump_duration = 3000 -- Initial jump duration. This decreases when hurt at second phase.

function enemy:on_created()
  self:create_sprite("citizens/blacksmith")
  self:set_life(30) -- 30 hits during all three phases.
  self:set_damage(1) -- REMARK: damage increases in latter phases.
  self:set_size(32, 16)
  self:set_origin(16, 13)
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(false)
  self:restart()
end

local function quake()
  local map = enemy:get_map()
  local cam = map:get_camera()
  local dir = 0
  local number_quakes = 6 -- Number of quakes.
  local it = 1 -- Current iteration (current quake).
  cam:start_manual()
  sol.audio.play_sound("running_obstacle")
  local function create_movement(direction)
    local m = sol.movement.create("straight")
    local angle = math.pi * dir
    m:set_speed(100)
    m:set_max_distance(6)
    m:set_angle(angle)
    m:set_ignore_obstacles(true)
    return m
  end
  -- First camera movement:
  local m = create_movement(dir)
  m:set_max_distance(3)
  dir = 1 - dir; it = it + 1 -- Update direction and iterator for next quake.
  number_quakes = number_quakes - 1
  m:start(cam)
  -- More camera movements:
  sol.timer.start(map, 50, function()
    local m = create_movement(dir)
    m:start(cam)
    if it <= number_quakes then
      dir = 1 - dir; it = it + 1 -- Update direction and iterator for next quake.
      return true
    else 
      -- Last camera movement:
      local m = create_movement(dir)
      m:set_max_distance(3)
      m:start(cam)
      -- Make camera work as usual.
      sol.timer.start(map, 50, function()
        cam:start_tracking(map:get_hero())
      end)
    end
  end)
end

local function jump(properties)
  local p = properties
  local e, max_height, jump_duration, dx, dy = p.entity_jumping, p.max_height, p.jump_duration, p.dx, p.dy
  local function f(t) -- Shifting function.
    return math.floor(4 * max_height * (t / jump_duration - (t / jump_duration) ^ 2))
  end
  -- Start jump
  sol.audio.play_sound("jump")
  local t = 0
  local refreshing_time = 10
  local sprite = e:get_sprite()
  sol.timer.start(e, refreshing_time, function() -- Update shift each 10 milliseconds.
    sprite:set_xy(0, -f(t))
    t = t + refreshing_time
    if t > jump_duration then return false 
    else return true end
  end)
  -- Create movement.
  if dx ~= 0 or dy ~= 0 then
    local m = sol.movement.create("straight")
    local angle = sol.main.get_angle(0, 0, dx, dy)
    local jumping_speed = math.floor( 1000 * sol.main.get_distance(0, 0, dx, dy) / jump_duration )
    m:set_speed(jumping_speed)
    m:set_angle(angle)
    m:start(e)
  end
  -- Update sprite animations.
  local shadow = enemy:create_sprite("shadows/shadow_huge_dynamic") -- Create shadow.
  local frame_delay = math.floor( jump_duration / shadow:get_num_frames() )
  shadow:set_frame_delay(frame_delay)
  enemy:get_sprite():set_animation("jump")
  sol.timer.start(enemy, math.floor( jump_duration * (4/10) ), function()
    enemy:get_sprite():set_animation("float")
    sol.timer.start(enemy, math.floor( jump_duration * (3/10) ), function()
      enemy:get_sprite():set_animation("fall")
      sol.timer.start(enemy, math.floor( jump_duration * (3/10) ), function()
        enemy:stop_movement()
        enemy:remove_sprite(shadow) -- Remove shadow.
        state = "stomp"
        enemy:set_can_attack(true) -- Allow to attack the hero.
        enemy:get_sprite():set_animation("stomp")
        quake() -- Start quake effect.
        sol.timer.start(enemy, 500, function()
          enemy:set_default_attack_consequences() -- Stop invincibility.        
          enemy:restart() -- Restart after the jump.
        end)
      end)
    end) 
  end)
end

function enemy:jump()
  state = "jump"
  self:set_invincible() -- Set invincible.
  self:set_can_attack(false) -- Do not allow to attack the hero.
  -- Call function to simulate jumps.
  local hx, hy, _ = self:get_map():get_hero():get_position()
  local ex, ey, _ = self:get_position()
  local dx, dy = hx - ex, hy - ey
  local prop = {entity_jumping = self, max_height = 96, jump_duration = current_jump_duration, dx = dx, dy = dy}
  jump(prop)
end

-- Go to hero. After a few seconds start jumping.
function enemy:go_to_hero()
  self:get_sprite():set_animation("running")
  local h = self:get_game():get_hero()
  local m = sol.movement.create("target")
  m:set_speed(speed)
  function m:on_position_changed()
    local dir = enemy:get_direction4_to(h)
    enemy:get_sprite():set_direction(dir)
  end
  function m:on_finished() enemy:restart() end
  m:start(enemy)
end

-- Jump to hero. After a few seconds start jumping.
function enemy:jump_to_hero()
  self:get_sprite():set_animation("running")
  local h = self:get_game():get_hero()
  local m = sol.movement.create("target")
  m:set_speed(speed)
  function m:on_position_changed()
    local dir = enemy:get_direction4_to(h)
    enemy:get_sprite():set_direction(dir)
  end
  m:start(enemy)
  sol.timer.start(enemy, 500, function() 
    enemy:stop_movement()
    enemy:jump()
  end)
end

-- Hammer to hero.
function enemy:hammer_to_hero()
  local s = self:get_sprite()
  s:set_animation("running")
  local h = self:get_game():get_hero()
  local m = sol.movement.create("target")
  m:set_speed(speed)
  function m:on_position_changed()
    local dir = enemy:get_direction4_to(h)
    s:set_direction(dir)
  end
  m:start(enemy)
  sol.timer.start(enemy, 1000, function()
    self:stop_movement()
    sol.audio.play_sound("sword4")
    local ham = enemy:create_sprite("citizens/blacksmith") -- Hammer sprite (invincible).
    enemy:set_invincible_sprite(ham)
    ham:set_animation("hammer_attack")
    ham:set_direction(s:get_direction())
    s:set_animation("blacksmith_attack", function()
      s:set_animation("blacksmith_attack_end")
      ham:set_animation("hammer_attack_end")
      sol.timer.start(enemy, 500, function()
        enemy:remove_sprite(ham)
        self:get_sprite():set_animation("running")
        self:restart()
      end)
    end)
  end)
end

function enemy:on_hurt()
  if phase == 1 then
    if speed < 80 then speed = speed + 5 -- Increase speed when the enemy is hurt.
    else -- Restore speed and start phase 2.
      phase = 2; speed = 50; self:restart()
      self:set_damage(2)
    end 
  elseif phase == 2 then
    if current_jump_duration > 1000 then
      current_jump_duration = current_jump_duration - 250 -- Increase jump speed when hurt.
    else -- Start phase 3.
      phase = 3; speed = 60; self:restart() 
      self:set_damage(4)
    end
  elseif phase == 3 then -- Remove hammer sprite after hurt, if any.
    local s = enemy:get_sprite()
    for _, sprite in enemy:get_sprites() do
      if sprite ~= s then enemy:remove_sprite(sprite) end
    end
  end
end

function enemy:on_restarted()
  if phase == 1 then self:go_to_hero()
  elseif phase == 2 then self:jump_to_hero()
  elseif phase == 3 then self:hammer_to_hero()
  end
end
