-- Creeper

explosion_soon = false

function event_appear()

  sol.enemy.set_life(10)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/creeper")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  m = sol.main.target_movement_create(40)
  sol.enemy.start_movement(m)
end

function event_restart()
  sol.main.timer_start(100, "check_hero", false)
end

function check_hero()

  near_hero = sol.enemy.get_distance_to_hero() < 50
  if near_hero and not explosion_soon then
    explosion_soon = true
    sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "hurt")
    sol.main.timer_start(400, "explode", false)
  else
    sol.main.timer_start(100, "check_hero", false)
  end
end

function explode()
  x, y = sol.enemy.get_position()
  layer = sol.enemy.get_layer()
  sol.main.play_sound("explosion")
  sol.map.explosion_create(x, y, layer)
  sol.map.explosion_create(x + 32, y, layer)
  sol.map.explosion_create(x + 24, y - 24, layer)
  sol.map.explosion_create(x, y - 32, layer)
  sol.map.explosion_create(x - 24, y - 24, layer)
  sol.map.explosion_create(x - 32, y, layer)
  sol.map.explosion_create(x - 24, y + 24, layer)
  sol.map.explosion_create(x, y + 32, layer)
  sol.map.explosion_create(x + 24, y + 24, layer)
  sol.map.enemy_remove(sol.enemy.get_name())
end


