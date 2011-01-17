-- A big butterfly boss from Newlink.

nb_eggs_to_create = 0
nb_eggs_created = 0

function event_appear()

  sol.enemy.set_life(6)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/papillosaur_king")
  sol.enemy.set_size(176, 96)
  sol.enemy.set_origin(88, 64)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("explosion", 1)
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_obstacle_behavior("flying")
end

function event_restart()

  sol.main.timer_start(2000, "egg_phase_soon", false)
  go()
end

function event_hurt(attack, life_lost)

  sol.main.timer_stop_all()
  if sol.enemy.get_life() <= 0 then
    -- I am dying: remove the minillosaur eggs
    sons_prefix = sol.enemy.get_name().."_minillosaur"
    sol.map.enemy_remove_group(sons_prefix)
    -- TODO sol.map.enemy_remove_sons or even sol.map.enemy_kill_sons
  end
end

function go()
  m = sol.main.random_path_movement_create(48)
  sol.enemy.start_movement(m)
end

function egg_phase_soon()

  sons_prefix = sol.enemy.get_name().."_minillosaur" 
  nb_sons = sol.map.enemy_get_group_count(sons_prefix)
  if nb_sons >= 5 then
    -- delay the egg phase if there are already too much sons
    sol.main.timer_start(5000, "egg_phase_soon", false)
  else
    sol.enemy.stop_movement()
    sol.main.timer_start(500, "egg_phase", false)
  end
end

function egg_phase()

  sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "preparing_egg")
  sol.main.play_sound("boss_charge")
  sol.main.timer_start(1500, "throw_egg", false)

  if sol.enemy.get_life() < 3 then
    nb_eggs_to_create = 3
  else
    nb_eggs_to_create = 1
  end

end

function throw_egg()

  -- create the egg
  nb_eggs_created = nb_eggs_created + 1
  egg_name = sol.enemy.get_name().."_minillosaur_"..nb_eggs_created
  sol.enemy.create_son(egg_name, "minillosaur_egg", 0, 16)
  sol.map.enemy_set_treasure(egg_name, "_none", 1, -1)
  sol.main.play_sound("boss_fireball")

  -- see what to do next
  nb_eggs_to_create = nb_eggs_to_create - 1
  if nb_eggs_to_create > 0 then
    -- throw another egg in 0.5 second
    sol.main.timer_start(500, "throw_egg", false)
  else
    -- finish the egg phase and schedule the next one in a few seconds
    sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "walking")
    duration = 3500 + (math.random(3) * 1000)
    sol.main.timer_start(duration, "egg_phase_soon", false)
    go()
  end
end

