-- A big butterfly boss from Newlink.

nb_eggs_to_create = 0
nb_eggs_created = 0

boss_starting_hp = 6
boss_movement_starting_speed = 50 -- Starting speed in pixels per second, it will gain 5 per hp lost.
boss_movement_speed = boss_movement_starting_speed

function event_appear()

  sol.enemy.set_life(boss_starting_hp)
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

  sol.main.timer_stop_all()
  sol.main.timer_start(egg_phase_soon, 2000)
  go()
end

function event_hurt(attack, life_lost)

  sol.main.timer_stop_all()
  local boss_hp = sol.enemy.get_life()
  if boss_hp <= 0 then
    -- I am dying: remove the minillosaur eggs
    local sons_prefix = sol.enemy.get_name().."_minillosaur"
    sol.map.enemy_remove_group(sons_prefix)
    -- TODO sol.map.enemy_remove_sons or even sol.map.enemy_kill_sons
  else
    boss_movement_speed = boss_movement_starting_speed
      + (boss_starting_hp - boss_hp) * 5
  end
end

function go()
  local m
  if sol.enemy.get_life() > 1 then
    m = sol.main.random_path_movement_create(boss_movement_speed)
  else
    -- The enemy is now desperate and angry against our hero
    m = sol.main.target_movement_create(boss_movement_speed)
  end
  sol.enemy.start_movement(m)
end

function egg_phase_soon()

  local sons_prefix = sol.enemy.get_name().."_minillosaur"
  local nb_sons = sol.map.enemy_get_group_count(sons_prefix)
  if nb_sons >= 5 then
    -- delay the egg phase if there are already too much sons
    sol.main.timer_start(egg_phase_soon, 5000)
  else
    sol.enemy.stop_movement()
    sol.main.timer_start(egg_phase, 500)
  end
end

function egg_phase()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "preparing_egg")
  sol.main.play_sound("boss_charge")
  sol.main.timer_start(throw_egg, 1500)

  -- The more the boss is hurt, the more it will throw eggs...
  nb_eggs_to_create = boss_starting_hp - sol.enemy.get_life() + 1

end

function throw_egg()

  -- create the egg
  nb_eggs_created = nb_eggs_created + 1
  local egg_name = sol.enemy.get_name().."_minillosaur_"..nb_eggs_created
  sol.enemy.create_son(egg_name, "minillosaur_egg_thrown", 0, 16)
  sol.map.enemy_set_treasure(egg_name, "_none", 1, -1)
  sol.main.play_sound("boss_fireball")

  -- see what to do next
  nb_eggs_to_create = nb_eggs_to_create - 1
  if nb_eggs_to_create > 0 then
    -- throw another egg in 0.5 second
    sol.main.timer_start(throw_egg, 500)
  else
    -- finish the egg phase
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "walking")
    -- Don't throw eggs when desperate!
    if sol.enemy.get_life() > 1 then
      -- Schedule the next one in a few seconds
      local duration = 3500 + (math.random(3) * 1000)
      sol.main.timer_start(egg_phase_soon, duration)
    end
    go()
  end
end

