-- Globul

function event_appear()

  sol.enemy.set_life(4)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/globul")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_hurt_sound_style("monster")
end

function event_movement_changed()

  m = sol.enemy.get_movement()
  direction4 = sol.main.movement_get_property(m, "displayed_direction")
  sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_direction(sprite, direction4)
end

function event_restart()
  go_random()
  sol.main.timer_start(1000, "check_hero", false)
end

function event_hurt()
  sol.main.timer_stop("check_hero")
end

function check_hero()

  near_hero = sol.enemy.get_distance_to_hero() < 100
  if near_hero and not hero_seen then
    hero_seen = true
    go_hero()
  elseif not near_hero and hero_seen then
    hero_seen = false
    go_random()
  end
  sol.main.timer_start(1000, "check_hero", false)
end

function go_random()
  m = sol.main.random_path_movement_create(32)
  sol.enemy.start_movement(m)
end

function go_hero()
  m = sol.main.target_movement_create(48)
  sol.enemy.start_movement(m)
end


