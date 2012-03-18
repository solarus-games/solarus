-- Snap Dragon

local timer

function event_appear()

  sol.enemy.set_life(3)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/snap_dragon")
  sol.enemy.set_hurt_style("normal")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
end

function event_restart()

  go_random()
end

function event_movement_finished(movement)

  go_random()
end

function event_obstacle_reached()

  go_random()
end

function event_hurt(attack, life_lost)

  if timer ~= nil then
    timer:stop()
    timer = nil
  end
end

function go_random()

  -- random diagonal direction
  local rand4 = math.random(4)
  local direction8 = rand4 * 2 - 1
  local angle = direction8 * math.pi / 4
  local m = sol.movement.straight_movement_create(48, angle)
  m:set_property("max_distance", 24 + math.random(96))
  sol.enemy.start_movement(m)

  local sprite = sol.enemy.get_sprite()
  sprite:set_direction(rand4 - 1)

  if timer ~= nil then
    timer:stop()
  end
  timer = sol.timer.start(300 + math.random(1500), function()
    sprite:set_animation("bite")
  end)
end

function event_sprite_animation_finished(sprite, animation)

  if animation == "bite" then
    sprite:set_animation("walking")
  end
end

