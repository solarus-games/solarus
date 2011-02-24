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

  distance_to_hero = sol.enemy.get_distance_to_hero()
  near_hero = distance_to_hero < 40

  if not near_hero and distance_to_hero < 60 then
    x, y = sol.enemy.get_position()
    hero_x, hero_y = sol.map.hero_get_position()
    if hero_y < y and y - hero_y >= 40 then
      near_hero = true
    end
  end

  if near_hero and not explosion_soon then
    explosion_soon = true
    sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "hurt")
    sol.main.play_sound("creeper")
    sol.main.timer_start(700, "explode", false)
  else
    sol.main.timer_start(100, "check_hero", false)
  end
end

function explode()
  x, y = sol.enemy.get_position()
  layer = sol.enemy.get_layer()
  sol.main.play_sound("explosion")
  sol.map.explosion_create(x, y - 16, layer)
  sol.map.explosion_create(x + 32, y - 16, layer)
  sol.map.explosion_create(x + 24, y - 40, layer)
  sol.map.explosion_create(x, y - 48, layer)
  sol.map.explosion_create(x - 24, y - 40, layer)
  sol.map.explosion_create(x - 32, y - 16, layer)
  sol.map.explosion_create(x - 24, y + 8, layer)
  sol.map.explosion_create(x, y + 16, layer)
  sol.map.explosion_create(x + 24, y + 8, layer)
  sol.map.enemy_remove(sol.enemy.get_name())
end


