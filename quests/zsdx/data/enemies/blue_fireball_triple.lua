-- A triple bouncing fireball that the hero cannot fight

local speed = 192
local bounces = 0
local max_bounces = 8
local sprite2 = nil
local sprite3 = nil

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(8)
  sol.enemy.create_sprite("enemies/blue_fireball_triple")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 8)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "custom")

  -- two smaller fireballs just for the displaying
  sprite2 = sol.main.sprite_create("enemies/blue_fireball_triple")
  sol.main.sprite_set_animation(sprite2, "small")
  sprite3 = sol.main.sprite_create("enemies/blue_fireball_triple")
  sol.main.sprite_set_animation(sprite3, "tiny")
end

function event_restart()

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  local angle = sol.main.get_angle(x, y, hero_x, hero_y - 5)
  local m = sol.main.straight_movement_create(speed, angle)
  --sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
end

function event_obstacle_reached()

  if bounces < max_bounces then

    -- compute the bouncing angle
    -- (works good with horizontal and vertical walls)
    local m = sol.enemy.get_movement()
    local angle = sol.main.movement_get_property(m, "angle")
    if not sol.main.movement_test_obstacles(m, 1, 0)
      and not sol.main.movement_test_obstacles(m, -1, 0) then
      angle = -angle
    else
      angle = math.pi - angle
    end

    sol.main.movement_set_property(m, "angle", angle)
    sol.main.movement_set_property(m, "speed", speed)

    bounces = bounces + 1
    speed = speed + 48
  else
    sol.map.enemy_remove(sol.enemy.get_name())
  end
end

function event_custom_attack_received(attack, sprite)

  if attack == "sword" then
    -- explode
    local x, y, layer = sol.enemy.get_position()
    sol.main.play_sound("explosion")
    sol.game.remove_life(4)
    sol.map.explosion_create(x, y, layer)
    sol.map.enemy_remove(sol.enemy.get_name())
  end
end

function event_pre_display()

  local m = sol.enemy.get_movement()
  local angle = sol.main.movement_get_property(m, "angle")
  local x, y = sol.enemy.get_position()

  local x2 = x - math.cos(angle) * 12
  local y2 = y + math.sin(angle) * 12

  local x3 = x - math.cos(angle) * 24
  local y3 = y + math.sin(angle) * 24

  sol.map.sprite_display(sprite2, x2, y2)
  sol.map.sprite_display(sprite3, x3, y3)
end

