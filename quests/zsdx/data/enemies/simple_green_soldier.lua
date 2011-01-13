-- Simple green soldier: a stupid soldier with a small sword
-- who goes in a random direction.
-- Unlike the normal green soldier, he cannot see or follow the hero.

-- Properties
life = 2
damage = 2
sprite = "enemies/simple_green_soldier"
size = {16, 16}
origin = {8, 13}

-- The enemy appears: create its movement
function event_appear()
  m = sol.main.temporal_movement_create(0, 0, 0)
  sol.enemy.start_movement(m)
end

-- The enemy was stopped for some reason and should restart
function event_restart()
  direction4 = math.random(4)
  go(direction4)
end

-- An obstacle is reached: stop for a while, looking to a next direction
function event_obstacle_reached()

  -- if the enemy was walking, look to the left or to the right
  sprite = sol.enemy.get_sprite()
  if sol.main.sprite_get_animation(sprite) == "walking" then
    look_left_or_right()
  end
end

-- The movement is finished: stop for a while, looking to a next direction
function event_movement_finished(movement)
  -- same thing as when an obstacle is reached
  event_obstacle_reached()
end

--  The animation of a sprite is finished
function event_sprite_animation_finished(sprite, animation)

  -- if the enemy was stopped and looking to a direction, go to that direction
  direction = sol.main.sprite_get_direction(sprite)
  if animation == "stopped_watching_left" then
    go((direction + 1) % 4)
  elseif animation == "stopped_watching_right" then
    go((direction + 3) % 4)
  end
end

-- Makes the soldier walk towards a direction
function go(direction4)

  -- set the sprite
  sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "walking")
  sol.main.sprite_set_direction(sprite, direction4)

  -- set the movement
  m = sol.enemy.get_movement()
  seconds = 2 + math.random(3)
  sol.main.movement_set_property(m, "duration", seconds * 1000)
  sol.main.movement_set_property(m, "speed", 40)
  sol.main.movement.set_property(m, "angle", direction4 * math.pi / 2)
end

-- Makes the soldier look to its left or to its right (random choice)
function look_left_or_right()

  if math.random(2) == 0 then
    sol.main.sprite_set_animation(sprite, "stopped_watching_left")
  else
    sol.main.sprite_set_animation(sprite, "stopped_watching_right")
  end
end

