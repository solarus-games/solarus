-- Pike that always moves, horizontally or vertically
-- depending on its direction

local recent_obstacle = 0

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/pike_auto")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_can_hurt_hero_running(true)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("thrown_item", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
end

function event_restart()

  local sprite = sol.enemy.get_sprite()
  local direction4 = sol.main.sprite_get_direction(sprite)
  local m = sol.main.path_movement_create(tostring(direction4 * 2), 192)
  sol.main.movement_set_property(m, "loop", true)
  sol.enemy.start_movement(m)
end

function event_obstacle_reached()

  local sprite = sol.enemy.get_sprite()
  local direction4 = sol.main.sprite_get_direction(sprite)
  sol.main.sprite_set_direction(sprite, (direction4 + 2) % 4)

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  if recent_obstacle == 0
      and math.abs(x - hero_x) < 184
      and math.abs(y - hero_y) < 144 then
    sol.main.play_sound("sword_tapping")
  end

  recent_obstacle = 8
  sol.enemy.restart()
end

function event_position_changed()

  if recent_obstacle > 0 then
    recent_obstacle = recent_obstacle - 1
  end
end

