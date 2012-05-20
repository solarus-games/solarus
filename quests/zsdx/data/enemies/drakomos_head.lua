-- Drakomos head

local nb_fire_created = 0
local max_fire_created = 10
local initial_xy = {}
local timers = {}

function event_appear()

  sol.enemy.set_life(16)
  sol.enemy.set_damage(16)
  sol.enemy.create_sprite("enemies/drakomos_head")
  sol.enemy.set_size(56, 56)
  sol.enemy.set_origin(28, 40)
  sol.enemy.set_hurt_style("boss")
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_no_treasure()

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("thrown_item", 1)

  initial_xy.x, initial_xy.y = sol.enemy.get_position()
end

function event_restart()

  local m = sol.movement.create("target")
  m:set_speed(48)
  m:set_target(initial_xy.x, initial_xy.y)
  sol.enemy.start_movement(m)
  for _, t in ipairs(timers) do t:stop() end

  nb_fire_created = 0
  timers[#timers + 1] = sol.main:start_timer(2000 + math.random(8000), function()
    sol.enemy.stop_movement()
    local sprite = sol.enemy.get_sprite()
    sprite:set_animation("preparing_fire")
    sol.audio.play_sound("lamp")
    timers[#timers + 1] = sol.main:start_timer(500, repeat_fire)
  end)
end

function event_hurt(attack, life_lost)

  if life_lost > 0 then
    for _, t in ipairs(timers) do t:stop() end
  end
end

function event_movement_finished(movement)

  local m = sol.movement.create("random")
  m:set_speed(48)
  m:set_max_distance(24)
  sol.enemy.start_movement(m)
end

function repeat_fire()

  if nb_fire_created < max_fire_created then
    nb_fire_created = nb_fire_created + 1
    local son_name = sol.enemy.get_name().."_son_"..nb_fire_created
    local angle_start = 2 * math.pi / 4
    local angle_end = 9 * math.pi / 4
    local angle = angle_start + nb_fire_created * (angle_end - angle_start) / max_fire_created
    sol.enemy.create_son(son_name, "fireball_simple", 0, 16)
    sol.enemy.send_message(son_name, tostring(angle))
    sol.audio.play_sound("lamp")
    timers[#timers + 1] = sol.main:start_timer(150, repeat_fire)
  else
    timers[#timers + 1] = sol.main:start_timer(500, sol.enemy.restart)
  end
end

