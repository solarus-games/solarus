-- Creeper

explosion_soon = false
going_hero = false

function event_appear()

  sol.enemy.set_life(5)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/creeper")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_attack_consequence("explosion", "ignored")
end

function event_restart()
  go_random()
  check_hero()
end

function check_hero()

  distance_to_hero = sol.enemy.get_distance_to_hero()
  near_hero = distance_to_hero < 40

  if distance_to_hero < 160 and not going_hero then
    go_hero()
  elseif distance_to_hero >= 160 and going_hero then
    go_random()
  end

  if not near_hero and distance_to_hero < 80 then
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
    sol.main.timer_start(600, "explode_if_near_hero", false)
  else
    sol.main.timer_start(100, "check_hero", false)
  end
end

function explode_if_near_hero()

  distance = sol.enemy.get_distance_to_hero()
  if distance >= 70 then
    -- cancel the explosion
    explosion_soon = false
    sol.main.timer_start(400, "check_hero", false)
    sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "walking")
  else
    -- explode
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
end

function go_random()
  m = sol.main.random_path_movement_create(40)
  sol.enemy.start_movement(m)
  going_hero = false
end

function go_hero()
  m = sol.main.target_movement_create(40)
  sol.enemy.start_movement(m)
  going_hero = true
end

