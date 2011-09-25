-- A giant tree boss from Newlink

nb_sons_created = 0
nb_sons_immobilized = 0
vulnerable = false

function event_appear()

  sol.enemy.set_life(8)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/master_arbror")
  sol.enemy.set_size(264, 312)
  sol.enemy.set_origin(132, 269)
  sol.enemy.set_pushed_back_when_hurt(true)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
end

function event_restart()

  local sprite = sol.enemy.get_sprite()
  if not vulnerable then
    go()
  else
    sol.main.sprite_set_animation(sprite, "vulnerable")
  end
end

function go()

  local m = sol.main.random_movement_create(16)
  sol.main.movement_set_property(m, "max_distance", 16)
  --sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
  sol.main.timer_stop_all()
  sol.main.timer_start(4000, "prepare_son")
  print("prepare son in 4s")
end

function event_hurt(attack, life_lost)

  if sol.enemy.get_life() <= 0 then
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation_ignore_suspend(sprite, true)
    sol.map.dialog_start("dungeon_3.arbror_killed")
  end
end

function prepare_son()

  print("prepare son")
  son_prefix = sol.enemy.get_name().."_son"
  if sol.map.enemy_get_group_count(son_prefix) < 4 then
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "preparing_son")
    sol.main.play_sound("hero_pushes")
    sol.main.timer_start(1000, "create_son")
    sol.enemy.stop_movement()
  end
end

function create_son()

  x = math.random(-7, 7) * 16

  nb_sons_created = nb_sons_created + 1
  son_name = sol.enemy.get_name().."_son_"..nb_sons_created
  sol.enemy.create_son(son_name, "arbror_root", x, 96)
  sol.main.play_sound("stone")
end

function event_sprite_animation_finished(sprite, animation)

  local sprite = sol.enemy.get_sprite()
  if animation == "preparing_son" then
    sol.main.sprite_set_animation(sprite, "walking")
    sol.enemy.restart()
  elseif animation == "son_immobilized" then

    if nb_sons_immobilized >= 4
        and not vulnerable then

      vulnerable = true
      sol.enemy.set_attack_consequence("sword", 1)
      sol.enemy.set_attack_consequence("arrow", 2)
      sol.enemy.stop_movement()
      sol.enemy.remove_life(1)
      sol.main.sprite_set_animation(sprite, "vulnerable")
      sol.main.play_sound("boss_hurt")
      sol.main.timer_stop_all()
      sol.main.timer_start(3000, "stop_vulnerable")
    else
      sol.main.sprite_set_animation(sprite, "walking")
    end
  end
end

function event_message_received(src_enemy, message)

  if message == "begin immobilized" then
    nb_sons_immobilized = nb_sons_immobilized + 1
    local sprite = sol.enemy.get_sprite()
    local animation = sol.main.sprite_get_animation(sprite)

    if animation == "preparing_son" then
      sol.enemy.restart()
    end

    sol.main.sprite_set_animation(sprite, "son_immobilized")

  elseif message == "end immobilized" then
    nb_sons_immobilized = nb_sons_immobilized - 1
  end
  print("sons immobilized: "..nb_sons_immobilized)
end

function stop_vulnerable()

  vulnerable = false
  nb_sons_immobilized = 0
  son_prefix = sol.enemy.get_name().."_son"
  sol.map.enemy_remove_group(son_prefix)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.restart()
end

