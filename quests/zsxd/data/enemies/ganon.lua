-- Ganon in the temple of stupidities (2F NE)

local being_pushed = false

function event_appear()

  sol.enemy.set_life(100000)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/ganon")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "custom")
  sol.enemy.set_attack_consequence("arrow", 1)
end

function event_restart()

  local m = sol.main.path_finding_movement_create(32)
  sol.enemy.start_movement(m)
end

function event_movement_changed()

  if not being_pushed then
    local m = sol.enemy.get_movement()
    local direction4 = sol.main.movement_get_property(m, "displayed_direction")
    local sprite = sol.enemy.get_sprite()
    if direction4 == 1 then
      sol.main.sprite_set_direction(sprite, 1)
    else
      sol.main.sprite_set_direction(sprite, 0)
    end
  end
end

function event_update()

  local x, y = sol.enemy.get_position()
  if x > 1216 and sol.enemy.get_life() > 0 then
    sol.enemy.set_position(x, y, 0) -- go to low layer
    sol.enemy.set_life(0)
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "hurt")
    sol.main.play_sound("boss_killed")
  elseif x > 1176 and not being_pushed then
    event_restart()
  end
end

function event_custom_attack_received(attack, sprite)

  if attack == "sword" then
    sol.main.play_sound("sword_tapping")
    being_pushed = true
    local x, y = sol.enemy.get_position()
    local hero_x, hero_y = sol.map.hero_get_position()
    local angle = sol.main.get_angle(hero_x, hero_y, x, y)
    local m = sol.main.straight_movement_create(128, angle)
    sol.main.movement_set_property(m, "max_distance", 26)
    sol.enemy.start_movement(m)
  end
end

function event_movement_finished(movement)

  if being_pushed then
    event_restart()
  end
end

function event_obstacle_reached()

  if being_pushed then
    event_restart()
  end
end

